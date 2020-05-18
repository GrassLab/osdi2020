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

enum TranslationAction {
    kVirtualToPhysical,
    kPhysicalToPFN
};

static Page page_frames[NUM_OF_PAGE_FRAMES];

static void setInUseBit(const size_t start, const size_t size) {
    for (size_t i = 0; i < size; ++i) {
        page_frames[start + i].in_use = 1;
    }
}

uint64_t translate(uint64_t origin, enum TranslationAction action) {
    switch (action) {
    case kVirtualToPhysical:
        // 0xffff000012345678
        // ->
        // 0x0000000012345678
        return origin & 0x0000ffffffffffff;
    case kPhysicalToPFN:
        // 0x0000000012345678
        // ->
        // 0x0000000012345
        // 0x3ffff == NUM_OF_PAGE_FRAMES - 1
        return ((origin & 0x3ffff000) >> 12);
    }
}

void initPageFrames(void) {
    memzero((uint64_t *)page_frames, (sizeof(Page) * NUM_OF_PAGE_FRAMES) / 8);

    // kernel page table (3-level translation: 4 page directory)
    // start from 0x0000
    setInUseBit(0, 4);

    // kernel image
    // start from 0x80000
    size_t start = 0x80000 / PAGE_SIZE;
    setInUseBit(start,
            ((uint64_t)&kernel_end & 0x0000fffffffff000) / PAGE_SIZE - start);

    // peripheral
    start = (MMIO_BASE & 0x0000fffffffff000) / PAGE_SIZE;
    setInUseBit(start, NUM_OF_PAGE_FRAMES);

    // interrupt stack @ MMIO_BASE - 0x1000
    // it has 4KB. since stack grows toward lower address -> -0x2000
    start = ((MMIO_BASE & 0x0000fffffffff000) - 0x2000 ) / PAGE_SIZE;
    setInUseBit(start, NUM_OF_PAGE_FRAMES);
}
