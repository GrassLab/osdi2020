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
        slubs[i].objectSize = 0;
        slubs[i].address = 0;
        slubs[i].next = NULL;
        slubs[i].prev = NULL;
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
        if (slubs[i].objectSize == 0) {
            return &slubs[i];
        }
    }
    return NULL;
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

    // return empty slub if still have one in page.
    Page *page = (allocator->pages)[allocator->pageCount-1];
    Slub *slub = allocator->slub;
    for (int j = 0; j < objectMaxCountInPage - 1; j++) {
        if (slub != NULL && slub->next == NULL) {
            Slub *newSlub = find_empty_slub();
            newSlub->objectSize = size;
            newSlub->address = slub->address + size;
            newSlub->next = NULL;
            newSlub->prev = slub;
            slub->next = newSlub;
            printf("[object allocate] page index: %d\n", page->index);
            printf("[object allocate] object size: %d\n", newSlub->objectSize);
            printf("[object allocate] address: %d\n", newSlub->address);
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
    allocator->pages[allocator->pageCount] = page;
    allocator->pageCount++;

    if (allocator->pageCount == 1) {
        allocator->slub = slub;
    }
    printf("[object allocate] page index: %d\n", page->index);
    printf("[object allocate] object size: %d\n", slub->objectSize);
    printf("[object allocate] address: %d\n", allocator->slub->address);
    return allocator->slub;
}

void free_object(Slub *slub) {
    slub->prev->next = slub->next;
    slub->next->prev = slub->prev;

    Allocator *allocator = find_allocator(slub->objectSize);
    if (allocator->pages[(allocator->pageCount)-1]->physicalAddr == slub->address) {
        
        free_page(allocator->pages[(allocator->pageCount)-1]);
        
        allocator->pages[(allocator->pageCount)-1] = NULL;
        allocator->pageCount--;
    }
    if (allocator->pageCount == 0) {
        allocator->slub = NULL;
    }

    slub->objectSize = 0;
    slub->address = 0;
    slub->next = NULL;
    slub->prev = NULL;
}

unsigned long allocate_memory() {

}

void free_memory() {
    
}