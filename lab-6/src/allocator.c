#include "allocator.h"
#include "memory.h"
#include "printf.h"

#define NULL 0

void allocator_init() {
    for (int i = 0; i < ALLOCATOR_NUMBER_MAX; i++) {
        allocators[i].slub = NULL;
        allocators[i].pageCount = 0;
        allocators[i].objectSize = 0;
        for (int j = 0; j < ALLOCATOR_PAGE_NUMBER_MAX; j++) {
            allocators[i].pages[j] = NULL;
        }
    }
    for (int i = 0; i < SLUB_NUMBER_MAX; i++) {
        slubs[i].used = SLUB_NOT_USED;
        slubs[i].objectSize = 0;
        slubs[i].address = 0;
        slubs[i].next = NULL;
        slubs[i].prev = NULL;
    }
}

void varied_size_init() {
    for (int i = 0; i <= 11; i++) {
       allocator_register(1 << i); 
    }
}

Allocator *find_allocator(int size) {
    for (int i = 0; i < ALLOCATOR_NUMBER_MAX; i++) {
        if (allocators[i].objectSize == size) {
            return &(allocators[i]);
        }
    }
    return NULL;
}

Slub* find_empty_slub() {
    for (int i = 0; i < SLUB_NUMBER_MAX; i++) {
        if (slubs[i].used == SLUB_NOT_USED && slubs[i].objectSize == 0) {
            return &slubs[i];
        }
    }
    return NULL;
}

Page* find_page_by_slub(Slub *slub) {
    Allocator *allocator = find_allocator(slub->objectSize);
    while (slub != NULL) {
        for (int i = 0;  i < allocator->pageCount; i++) {
            if (slub->address == ((allocator->pages)[i])->physicalAddr) {
                return (allocator->pages)[i];
            }
        }
        slub = slub->prev;
    }
}

void allocator_register(int size) {
    Allocator *allocator = find_allocator(size);
    if (allocator == NULL) {
        for (int i = 0; i < ALLOCATOR_NUMBER_MAX; i++) {
            if (allocators[i].objectSize == 0) {
                allocators[i].objectSize = size;
                return;
            }
        }
    }
}

Slub* allocate_object(int size) {
    Allocator *allocator = find_allocator(size);
    if (allocator == NULL) return;
    int objectMaxCountInPage = PAGE_SIZE / allocator->objectSize; 

    // return slub if middle of slub list have empty one.
    Slub *slub = allocator->slub;
    while(slub != NULL) {
        if (slub->used == SLUB_NOT_USED) {
            slub->used = SLUB_USED;
            Page *page = find_page_by_slub(slub);
            printf("[object allocate] page index: %d\n", page->index);
            printf("[object allocate] object size: %d\n", slub->objectSize);
            printf("[object allocate] address: %x\n", slub->address);
            return slub;
        }
        slub = slub->next;
    }

    // return empty slub if still have one in page.
    Page *page = (allocator->pages)[allocator->pageCount-1];
    slub = allocator->slub;
    for (int j = 0; j < objectMaxCountInPage - 1; j++) {
        if (slub != NULL && slub->next == NULL) {
            Slub *newSlub = find_empty_slub();
            newSlub->objectSize = size;
            newSlub->address = slub->address + size;
            newSlub->next = NULL;
            newSlub->prev = slub;
            newSlub->used = SLUB_USED;
            slub->next = newSlub;
            printf("[object allocate] page index: %d\n", page->index);
            printf("[object allocate] object size: %x\n", newSlub->objectSize);
            printf("[object allocate] address: %x\n", newSlub->address);
            return newSlub;
        } else if (slub != NULL) {
            slub = slub->next;
        }
    }

    // allocate a page if no slab remain
    page = get_page(0);
    slub = find_empty_slub();
    slub->objectSize = size;
    slub->address = page->physicalAddr;
    slub->used = SLUB_USED;
    slub->next = NULL;
    slub->prev = NULL;
    allocator->pages[allocator->pageCount] = page;
    allocator->pageCount++;

    if (allocator->pageCount == 1) {
        allocator->slub = slub;
    }
    printf("[object allocate] page index: %d\n", page->index);
    printf("[object allocate] object size: %x\n", slub->objectSize);
    printf("[object allocate] address: %x\n", slub->address);
    return slub;
}

void free_object(Slub *slub) {
    Allocator *allocator = find_allocator(slub->objectSize);
    Page *page = find_page_by_slub(slub);
    slub->used = SLUB_NOT_USED;

    printf("[object free] page index: %d\n", page->index);
    printf("[object free] page address: %x\n", page->physicalAddr);
    printf("[object free] slub address: %x\n", slub->address);
    printf("[object free] slub size: %x\n", slub->objectSize);

    int objectMaxCountInPage = PAGE_SIZE / allocator->objectSize; 
    while (slub->address != page->physicalAddr) {
        slub = slub->prev;
    }
    for (int i = 0; i < objectMaxCountInPage; i++) {
        // printf("...%d %d %d\n", slub, slub->used, slub->next);
        if (slub == NULL) break;
        if (slub->used == SLUB_USED) {
            return;
        }
        slub = slub->next;
    }
    
    printf("[page free] page index: %d\n", page->index);
    free_page(page);
}

unsigned long allocate_memory(int size) {
    int allocateObjectSize = 0;
    for (int i = 0; i <= 11; i++) {
        if ((size >> i) <= 0) {
            allocateObjectSize = (1 << i);
            Slub *slub = allocate_object(allocateObjectSize);
            printf("[memory allocator] allocate an object, size: %d\n", allocateObjectSize);
            return slub->address;
        }
    }

    for (int i = 0; i <= 12; i++) {
        if ((size >> 12 >> i) <= 0) {
            Page *page = get_page(i);
            printf("[memory allocator] allocate a page, index: %d, order: %d\n", page->index, i);
            return page->physicalAddr;
        }
    }
}

void free_memory(unsigned long address) {
    for (int i = 0; i < SLUB_NUMBER_MAX; i++) {
        if (slubs[i].address == address) {
            free_object(&slubs[i]);
            printf("[memory free] free an object, size: %d\n", slubs[i].objectSize);
        }
    }   
    for (int i = 0; i < ALLOCATOR_NUMBER_MAX; i++) {
        for (int j = 0; j < ALLOCATOR_PAGE_NUMBER_MAX; j++) {
            if (allocators[i].pages[j]->physicalAddr == address) {
                free_page(allocators[i].pages[j]);
                printf("[memory free] free an page, index: %d, order: %d\n", (allocators[i].pages[j])->index, (allocators[i].pages[j])->order);
            }
        }
    }
}