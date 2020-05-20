#ifndef __MMU_VMA_H
#define __MMU_VMA_H

#include <stdint.h>

typedef struct __Page {
    uint64_t in_use;

    // concatenate related pages for reclaiming them more easily
    struct __Page *next;
} Page;

enum TranslationAction {
    kVirtualToPhysical,
    kPhysicalToVirtual,
    kPhysicalToPFN,
    kPFNToPhysical,
    kPageDescriptorToPFN,
    kPageDescriptorToPhysical,
    kPageDescriptorToVirtual
};

void initPageFrames(void);
uint64_t translate(uint64_t origin, enum TranslationAction action);
Page *allocPage(void);
void freePages(Page *page_frame);
Page *updatePageFramesForMappingStack(Page *pgd, Page *tail_page);
Page *updatePageFramesForMappingProgram(Page *pgd, Page *tail_page,
                                        uint64_t start);
uint64_t *getPhysicalofVirtualAddressFromPGD(Page *pgd, uint64_t addr);

#endif
