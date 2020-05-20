#include "mmu/vma.h"
#include "memzero.h"
#include "mmu/mmu.h"
#include "peripheral/base.h"

#include <stddef.h>
#include <stdint.h>

// 1GB / 4KB (RAM size / page size)
#define NUM_OF_PAGE_FRAMES (0x40000000 / PAGE_SIZE)

// symbol in linker script
extern uint64_t kernel_end;


static Page page_frames[NUM_OF_PAGE_FRAMES];

static void setInUseBit(const size_t start, const size_t end) {
    for (size_t i = start; i < end; ++i) {
        page_frames[i].in_use = 1;
    }
}

uint64_t translate(uint64_t origin, enum TranslationAction action) {
    switch (action) {
    case kVirtualToPhysical:
        // 0xffff000012345678
        // ->
        // 0x0000000012345678
        return origin & 0x0000ffffffffffff;
    case kPhysicalToVirtual:
        return origin | 0xffff000000000000;
    case kPhysicalToPFN:
        // 0x0000000012345678
        // ->
        // 0x0000000012345
        // 0x3ffff == NUM_OF_PAGE_FRAMES - 1
        return ((origin & 0x3ffff000) >> 12);
    case kPFNToPhysical:
        return ((origin << 12));
    case kPageDescriptorToPFN:
        return (origin - (uint64_t)page_frames) / sizeof(Page);
    case kPageDescriptorToPhysical:
        return translate(translate(origin, kPageDescriptorToPFN),
                         kPFNToPhysical);
    case kPageDescriptorToVirtual:
        return translate(translate(origin, kPageDescriptorToPhysical),
                         kPhysicalToVirtual);
    }
}

void initPageFrames(void) {
    memzero((uint64_t *)page_frames, (sizeof(Page) * NUM_OF_PAGE_FRAMES) / 8);

    // kernel page table (3-level translation: 4 page directory)
    // start from 0x0000
    setInUseBit(translate(0, kPhysicalToPFN),
                translate(0x4000, kPhysicalToPFN));

    // kernel image
    // start from 0x80000
    setInUseBit(
        translate(0x80000, kPhysicalToPFN),
        translate((uint64_t)&kernel_end & 0x0000fffffffff000, kPhysicalToPFN));

    // peripheral
    setInUseBit(translate((MMIO_BASE & 0x0000fffffffff000), kPhysicalToPFN),
                translate(0x40000000, kPhysicalToPFN));

    // interrupt stack @ MMIO_BASE - 0x1000
    // it has 4KB. since stack grows toward lower address -> -0x2000
    setInUseBit(
        translate((MMIO_BASE & 0x0000fffffffff000) - 0x2000, kPhysicalToPFN),
        translate((MMIO_BASE & 0x0000fffffffff000) - 0x1000, kPhysicalToPFN));
}

Page *allocPage(void) {
    for (size_t i = 0; i < NUM_OF_PAGE_FRAMES; ++i) {
        if (page_frames[i].in_use == 0) {
            page_frames[i].in_use = 1;
            memzero((uint64_t *)translate((uint64_t)&page_frames[i],
                                          kPageDescriptorToVirtual),
                    PAGE_SIZE / 8);

            return &page_frames[i];
        }
    }

    sendStringUART("Out of memory page\n");
    return NULL;
}

static void freePage(Page *page_frame) {
    page_frame->in_use = 0;
    page_frame->next = NULL;
}

void freePages(Page *page_frame) {
    if (page_frame == NULL) {
        return;
    }

    Page *next = page_frame->next;
    while (next) {
        freePage(page_frame);
        page_frame = next;
        next = page_frame->next;
    }
    freePage(page_frame);
}
