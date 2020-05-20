#include "mmu/vma.h"
#include "memzero.h"
#include "mmu/mmu.h"
#include "peripheral/base.h"

#include <stddef.h>
#include <stdint.h>

// 1GB / 4KB (RAM size / page size)
#define NUM_OF_PAGE_FRAMES (0x40000000 / PAGE_SIZE)

// symbol in linker script
extern uint64_t kernel_start; // physical
extern uint64_t kernel_end; // virtualized

extern const uint64_t kDefaultStackVirtualAddr;

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
    // start from kernel_start (physical)
    setInUseBit(
        translate((uint64_t)&kernel_start, kPhysicalToPFN),
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

static uint64_t getPGDIndex(const uint64_t virt_addr) {
    return (virt_addr & (0b111111111l << 39)) >> 39;
}

static uint64_t getPUDIndex(const uint64_t virt_addr) {
    return (virt_addr & (0b111111111l << 30)) >> 30;
}

static uint64_t getPMDIndex(const uint64_t virt_addr) {
    return (virt_addr & (0b111111111l << 21)) >> 21;
}

static uint64_t getPTEIndex(const uint64_t virt_addr) {
    return (virt_addr & (0b111111111l << 12)) >> 12;
}

static void updateNextLevelTableIfNecessary(uint64_t *cur_table,
                                            const size_t index,
                                            Page **indirect_tail_page,
                                            uint64_t **indirect_next_table) {
    if (cur_table[index] == 0lu) {
        Page *new_page = allocPage();
        (*indirect_tail_page)->next = new_page;
        *indirect_tail_page = new_page;
        *indirect_next_table =
            (uint64_t *)translate((uint64_t)new_page, kPageDescriptorToVirtual);

        cur_table[index] =
            translate((uint64_t)*indirect_next_table, kVirtualToPhysical) |
            PD_TABLE;
    } else {
        *indirect_next_table = (uint64_t *)translate(
            (cur_table[index] & 0x0000fffffffff000), kPhysicalToVirtual);
    }
}

// populate default user stack address related tables
Page *updatePageFramesForMappingStack(Page *pgd, Page *tail_page) {
    uint64_t *pgd_page_frame =
        (uint64_t *)translate((uint64_t)pgd, kPageDescriptorToVirtual);
    uint64_t *pud_page_frame;
    uint64_t *pmd_page_frame;
    uint64_t *pte_page_frame;
    const size_t pgd_index = getPGDIndex(kDefaultStackVirtualAddr);
    const size_t pud_index = getPUDIndex(kDefaultStackVirtualAddr);
    const size_t pmd_index = getPMDIndex(kDefaultStackVirtualAddr);
    const size_t pte_index = getPTEIndex(kDefaultStackVirtualAddr);

    updateNextLevelTableIfNecessary(pgd_page_frame, pgd_index, &tail_page,
                                    &pud_page_frame);

    updateNextLevelTableIfNecessary(pud_page_frame, pud_index, &tail_page,
                                    &pmd_page_frame);

    updateNextLevelTableIfNecessary(pmd_page_frame, pmd_index, &tail_page,
                                    &pte_page_frame);

    // set block in PTE
    // default give stack 8KB (kDefaultStackVirtualAddr +- 0x1000)
    Page *stack1 = allocPage();
    Page *stack2 = allocPage();

    tail_page->next = stack1;
    stack1->next = stack2;
    tail_page = stack2;

    pte_page_frame[pte_index - 1] =
        translate((uint64_t)stack1, kPageDescriptorToPhysical) |
        STACK_BLOCK_ATTR;
    pte_page_frame[pte_index] =
        translate((uint64_t)stack2, kPageDescriptorToPhysical) |
        STACK_BLOCK_ATTR;

    return tail_page;
}

// TODO: this should allocate pages for placing user program (r3-3, r3-4)
// one page once a time
Page *updatePageFramesForMappingProgram(Page *pgd, Page *tail_page,
                                        uint64_t start) {
    uint64_t *pgd_page_frame =
        (uint64_t *)translate((uint64_t)pgd, kPageDescriptorToVirtual);
    uint64_t *pud_page_frame;
    uint64_t *pmd_page_frame;
    uint64_t *pte_page_frame;

    updateNextLevelTableIfNecessary(pgd_page_frame, getPGDIndex(start),
                                    &tail_page, &pud_page_frame);

    updateNextLevelTableIfNecessary(pud_page_frame, getPUDIndex(start),
                                    &tail_page, &pmd_page_frame);

    updateNextLevelTableIfNecessary(pmd_page_frame, getPMDIndex(start),
                                    &tail_page, &pte_page_frame);

    tail_page->next = allocPage();
    tail_page = tail_page->next;
    pte_page_frame[getPTEIndex(start)] =
        translate((uint64_t)tail_page, kPageDescriptorToPhysical) |
        BINARY_BLOCK_ATTR;

    return tail_page;
}

static uint64_t *getNextLevelTable(uint64_t *cur_table, const size_t index) {
    if (cur_table[index] == 0lu) {
        return NULL;
    } else {
        return (uint64_t *)translate((cur_table[index] & 0x0000fffffffff000),
                                     kPhysicalToVirtual);
    }
}

uint64_t *getPhysicalofVirtualAddressFromPGD(Page *pgd, uint64_t addr) {
    uint64_t *pgd_page_frame =
        (uint64_t *)translate((uint64_t)pgd, kPageDescriptorToVirtual);
    uint64_t *pud_page_frame;
    uint64_t *pmd_page_frame;
    uint64_t *pte_page_frame;

    if ((pud_page_frame =
             getNextLevelTable(pgd_page_frame, getPGDIndex(addr))) == NULL) {
        return NULL;
    }

    if ((pmd_page_frame =
             getNextLevelTable(pud_page_frame, getPUDIndex(addr))) == NULL) {
        return NULL;
    }

    if ((pte_page_frame =
             getNextLevelTable(pmd_page_frame, getPMDIndex(addr))) == NULL) {
        return NULL;
    }

    return (uint64_t *)translate(pte_page_frame[getPTEIndex(addr)],
                                 kPhysicalToVirtual);
}
