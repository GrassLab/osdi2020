#include "mmu/page.h"
#include "mmu/buddy.h"
#include "peripheral/base.h"
#include "MiniUart.h"

#include <stddef.h>

typedef struct __Page {
    Block block;
    struct __Page *next;
} Page;

// record prev_base in *base for boosting the performance
typedef struct __PageManager {
    Page *base;
    Page *end;

    // used as bit map, uint32_t [(512MB / 4KB) / 32 (bits/per uint32_t)],
    // start from 0x500000
    // which is b/t Buddy System base (0x100000~0x400000) and BuddiesBase (0x600000~)
    uint32_t *bits;
} PageManager;

static PageManager manager = { (Page *)BUDDY_SYSTEM_BASE,
                               (Page *)BUDDY_SYSTEM_BASE + 0x20000000,
                               (uint32_t *)0x500000};
static const size_t kNumOfBitMaps = (512 * 1024) / 4 / 32;

static void initPages(void) {
    for (size_t i = 0; i < kNumOfBitMaps; ++i) {
        manager.bits[i] = 0xffffffff;
    }
}

static Page *getFreePage(void) {
    for (size_t i = 0; i < kNumOfBitMaps; ++i) {
        // ffs returns one plus the index of the least significant 1-bit
        size_t page_index = __builtin_ffs(manager.bits[i]);
        if (!page_index) {
            continue;
        }
        // for matching 0-base (ref comment of ffs above)
        --page_index;

        // unset bit
        manager.bits[i] ^= 1 << page_index;

        page_index += i * sizeof(manager.bits[0]) * 8;
        return &manager.base[page_index];
    }
    sendStringUART("Out of pages\n");
    return NULL;
}

static void insertPage(Page **indirect_head, Page *page) {
    page->next = *indirect_head;
    *indirect_head = page;
}

static void pushFrontPage(Page **indirect_head, Block *block) {
    Page *new_page = getFreePage();
    new_page->block = *block;
    insertPage(indirect_head, new_page);
}

static size_t getPageIndex(Page *page) {
    return ((uint64_t)page - (uint64_t)manager.base) / sizeof(Page);
}

static size_t getBitMapsIndex(size_t page_index) {
    // page_index / (1 << sizeof(bits[0]))
    return page_index >> (sizeof(manager.bits[0]) + 1);
}

static void deallocatePage(Page *page) {
    size_t page_index = getPageIndex(page);
    size_t bit_map_index = getBitMapsIndex(page_index);

    // reset bit
    manager.bits[bit_map_index] ^= 1 << (page_index & 0b11111);
}

static void popFrontPage(Page **indirect_head) {
    Page *next = (*indirect_head)->next;
    deallocatePage(*indirect_head);
    *indirect_head = next;
}

static void erasePage(Page **indirect_next, Page *page) {
    *indirect_next = page->next;
    deallocatePage(page);
}

static void showPage(Page *page) {
    sendStringUART("[");
    sendHexUART(page->block.lower);
    sendStringUART(", ");
    sendHexUART(page->block.upper);
    sendStringUART("]");
}

static void showPageList(Page *head) {
    if (head == NULL) {
        return;
    }

    Page *next = head->next;
    while (next) {
        showPage(head);
        sendStringUART(", ");
        head = next;
        next = head->next;
    }
    showPage(head);
}

static Page *allocatePage(Page **indirect_head) {
    Page *page = *indirect_head;
    *indirect_head = (*indirect_head)->next;
    page->next = NULL;
    return page;
}

__attribute__((const))
static Block *getBlockFromPage(Page *page) {
    return &page->block;
}

__attribute__((const))
static Page *getNextPage(Page *page) {
    return page->next;
}

__attribute__((const))
static Page **getIndirectNextPage(Page *page) {
    return &page->next;
}

static uint64_t getPFN(uint64_t addr) {
    // max PFN << 12 for masking
    // 0x3ffff000 == 1GB / 4KB == # of pages
    const uint64_t kMaxPFN = 0x3ffff000;
    return (addr & kMaxPFN) >> 12;
}

__attribute__((const))
static bool inPage(Page *page, uint64_t addr) {
    uint64_t pfn_of_page_low = getPFN(page->block.lower);
    uint64_t pfn_of_page_high = getPFN(page->block.upper);
    uint64_t pfn_of_addr = getPFN(addr);
    return (pfn_of_page_low <= pfn_of_addr) && (pfn_of_addr < pfn_of_page_high);
}

PageType gPage = {
    .init = initPages,
    .pushFront = pushFrontPage,
    .popFront = popFrontPage,
    .insert = insertPage,
    .erase = erasePage,
    .showList = showPageList,
    .allocate = allocatePage,
    .deallocate = deallocatePage,
    .inPage = inPage,
    .getBlock = getBlockFromPage,
    .getNext = getNextPage,
    .getIndirectNext = getIndirectNextPage
};
