#include "mmu/kmalloc.h"
#include "mmu/buddy.h"
#include "mmu/mmu.h"
#include "mmu/slab.h"
#include "mmu/page.h"
#include "MiniUart.h"

static uint64_t token_array[PAGE_SIZE / 8];
static Page *large_size_page_list;
static Page **indirect_large_size_page_list = &large_size_page_list;

static const size_t kMetaDataSlots = 1;

void kmalloc_init(void) {
    for (size_t i = 0; i < PAGE_SIZE / 8; ++i) {
        token_array[i] = 0lu;
    }
}

void kmalloc_fini(void) {
    for (size_t i = 0; i < PAGE_SIZE / 8; ++i) {
        if (token_array[i]) {
            gSlab.delete(token_array[i]); 
        }
    }
}

void *kmalloc(size_t size) {
    if (size == 0) {
        return NULL;
    }

    // +8 for placing meta data for kmalloc
    // chunk ptr ->    |      chunk size      |
    // returned ptr -> |      user data       |
    //                 |         ...          |
    size_t aligned_size = alignSize(size) + 8;    
    uint64_t *chunk;

    if (aligned_size < PAGE_SIZE) {
        size_t order = aligned_size / 8;
        if (!token_array[order]) {
            token_array[order] = gSlab.regist(aligned_size);
        }
        
        chunk = gSlab.allocate(token_array[order]);
    } else {
        Page *page = gBuddy.allocate(aligned_size);

        gPage.insert(indirect_large_size_page_list, page);

        chunk = (uint64_t *)gPage.getBlock(page)->lower;
    }

    chunk[0] = aligned_size;

    sendStringUART("[kmalloc] malloc size ");
    sendHexUART(size);
    sendStringUART(", give it size ");
    sendHexUART(aligned_size - 8);
    sendUART('\n');

    return chunk + kMetaDataSlots;
}

void kfree(void *ptr) {
    uint64_t *chunk = (uint64_t *)((uint64_t)ptr - kMetaDataSlots * 8);
    sendStringUART("[kmalloc] free size ");
    sendHexUART(chunk[0] - 8);
    sendUART('\n');

    if (chunk[0] < PAGE_SIZE) {
        gSlab.deallocate(chunk);
    } else {
        Page **indirect_next = indirect_large_size_page_list;
        Page *cur_page = large_size_page_list;

        while (cur_page) {
            if (gPage.inPage(cur_page, (uint64_t)chunk)) {
                gPage.erase(indirect_next, cur_page);
                gBuddy.deallocate(cur_page);
                break;
            }

            indirect_next = gPage.getIndirectNext(cur_page);
            cur_page = gPage.getNext(cur_page); 
        }
    }
}
