#include "mmu/buddy.h"
#include "mmu/mmu.h"
#include "mmu/page.h"
#include "peripheral/base.h"
#include "MiniUart.h"

#include <stddef.h>
#include <stdint.h>

// 4MB / 4K == 1024 pages == 2 ** 10 pages
#define MAX_ORDER 10
static const size_t kMaxOrder = MAX_ORDER;

const uint64_t kBuddySystemBase = BUDDY_SYSTEM_BASE;

// memory space for allocatable pages (size: 512 MB/0x20000000)
const uint64_t kTotalMemorySizeInMB = 8;
const uint64_t kBuddiesBase = 0x600000 + VIRTUAL_ADDRESS_BASE;
const uint64_t kMaxOrderSize = 0x400000;

typedef struct __Buddy {
    Page *pages[MAX_ORDER + 1];
    size_t order0_bit_index;
    size_t order10_bit_index;
} Buddy;

static Buddy main_buddy;

static size_t getIndex(size_t size) {
    if (size > 0xffffffff || size == 0) {
        sendStringUART("[Buddy] Shouldn't reach here! size shouldn't > 32bits or == 0\n");
        return 0;
    }
    const size_t kTotalBits = sizeof(unsigned int) * 8;
    // -1 for making bits range 0 ~ 31
    // warning: clz(0) is UB
    return (kTotalBits - 1) - __builtin_clz(size);
}

static void constructBuddy(void) {
    gPage.init();

    // initialize pages array
    for (size_t i = 0; i < kMaxOrder; ++i) {
        main_buddy.pages[i] = NULL;
    }
    // total memory 512 MB
    const size_t kNumOf4MBlocks = kTotalMemorySizeInMB / 4;
    uint64_t start = kBuddiesBase;
    for (size_t i = 0; i < kNumOf4MBlocks; ++i) {
        Block block = {start, start + /* 4MB */ kMaxOrderSize};
        gPage.pushFront(&main_buddy.pages[kMaxOrder], &block);
        start = block.upper;
    }

    main_buddy.order0_bit_index = getIndex(PAGE_SIZE);
    main_buddy.order10_bit_index = getIndex(PAGE_SIZE * (1 << 10));
}

void showBuddy(void) {
    static const char hex_chars[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a'};

    sendStringUART("======================================\n");
    sendStringUART("Buddy System:\n");
    for (size_t i = 0; i <= kMaxOrder; ++i) {
        sendStringUART("Order ");
        sendUART(hex_chars[i]);
        sendStringUART(" (");
        sendHexUART((1 << i) * 0x1000 /* i page size*/);
        sendStringUART(")");
        sendStringUART(": ");
        gPage.showList(main_buddy.pages[i]);
        sendUART('\n');
    }
    sendStringUART("======================================\n");
}

__attribute__((const))
static uint64_t translateOrderToBytes(size_t order) {
    uint64_t num_of_pages = 1 << order;
    return num_of_pages * PAGE_SIZE;
}

static Page *allocPageFromBuddy(size_t page_order) {
    Page *page;

    if (main_buddy.pages[page_order]) {
        page = gPage.allocate(&main_buddy.pages[page_order]);
        showBuddy();
        return page;
    }

    // find free buddy
    for (size_t i = page_order + 1; i <= kMaxOrder; ++i) {
        if (main_buddy.pages[i]) {
            size_t j = i;
            do {
                Page *free_page = main_buddy.pages[j];

                j--;
                Block first_half = {
                    gPage.getBlock(free_page)->lower,
                    gPage.getBlock(free_page)->lower + translateOrderToBytes(j)
                };
                Block second_half = {
                    gPage.getBlock(free_page)->lower + translateOrderToBytes(j),
                    gPage.getBlock(free_page)->upper
                };

                gPage.popFront(&main_buddy.pages[j + 1]);
                gPage.pushFront(&main_buddy.pages[j], &first_half);
                gPage.pushFront(&main_buddy.pages[j], &second_half);
            } while (j != page_order);

            page = gPage.allocate(&main_buddy.pages[page_order]);
            showBuddy();
            return page;
        }
    }

    sendStringUART("[Buddy] Out of memory!!!\n");
    return NULL;
}

static Page *allocateFromBuddy(size_t block_size) {
    sendStringUART("[Buddy] Allocate size: ");
    sendHexUART(block_size);
    sendUART('\n');

    if (block_size == 0) {
        return NULL;
    }

    size_t bit_index = getIndex(block_size);
    bit_index = (block_size & ((1 << bit_index) - 1)) ? bit_index + 1 : bit_index;
    if (bit_index > main_buddy.order10_bit_index) {
        sendStringUART("[Buddy] Over size! Maximum is 4MB!\n");
        return NULL;
    }

    // just allocate 1 page from order 0
    if (bit_index < main_buddy.order0_bit_index) {
        bit_index = main_buddy.order0_bit_index;
    }

    return allocPageFromBuddy(/* page_order */ bit_index - main_buddy.order0_bit_index);
}

static void deallocateFromBuddy(Page *page) {
    size_t block_size = gPage.getBlock(page)->upper - gPage.getBlock(page)->lower;
    size_t bit_index = getIndex(block_size);

    sendStringUART("[Buddy] Deallocate size: ");
    sendHexUART(block_size);
    sendUART('\n');

    if (block_size & ((1 << bit_index) - 1)) {
        sendStringUART("[Buddy] Weird block_size!\n");
        return;
    }

    // search for buddy from it's order list for coalescing
    const size_t page_order = bit_index - main_buddy.order0_bit_index;

    // cannot coalesce the chunk in max order list
    if (page_order != kMaxOrder) {
        Page **indirect_next = &main_buddy.pages[page_order];
        Page *cur_page = *indirect_next;

        uint64_t prev_buddy = gPage.getBlock(page)->lower - (1 << page_order) * PAGE_SIZE;
        uint64_t post_buddy = gPage.getBlock(page)->lower + (1 << page_order) * PAGE_SIZE;
        while (cur_page) {
            Block *block = gPage.getBlock(page);;
            if (gPage.getBlock(cur_page)->lower == prev_buddy) {
                block->lower = gPage.getBlock(cur_page)->lower;
            } else if (gPage.getBlock(cur_page)->lower == post_buddy) {
                block->upper = gPage.getBlock(cur_page)->upper;
            } else {
                indirect_next = gPage.getIndirectNext(cur_page);
                cur_page = gPage.getNext(cur_page);
                continue;
            }

            gPage.erase(indirect_next, cur_page);
            showBuddy();
            deallocateFromBuddy(page);
            return;
        }
    }

    // normal deallocate
    gPage.insert(&main_buddy.pages[page_order], page);
    showBuddy();
}

BuddyType gBuddy = {
    .construct = constructBuddy,
    .show = showBuddy,
    .allocate = allocateFromBuddy,
    .deallocate = deallocateFromBuddy
};
