#ifndef _MM_H
#define _MM_H
#include "peripherals/base.h"

#define VA_START                0xffff000000000000

#define PAGE_SHIFT              12
#define TABLE_SHIFT             9
#define SECTION_SHIFT           (PAGE_SHIFT + TABLE_SHIFT)
#define PAGE_SIZE               (1 << PAGE_SHIFT)
#define SECTION_SIZE            (1 << SECTION_SHIFT)

#define TABLE_MASK              ((1 << TABLE_SHIFT) - 1)

// 4MB (page table) + 512KB (kernel start) + xxx (kernel code)
// 1028 * 4096 + 2048 * 1024
// round to 6MB for convinence
#define LOW_MEMORY              0x600000 
#define HIGH_MEMORY             0x3f000000

#define NUM_PAGES               (HIGH_MEMORY >> PAGE_SHIFT)
#define FIRST_AVAIL_PAGE        (LOW_MEMORY >> PAGE_SHIFT)
#define LAST_PAGE               16384

#ifndef __ASSEMBLER__

void memzero(unsigned long src, unsigned long n);
void memcpy(unsigned long dst, unsigned long src, unsigned long n);
unsigned long allocate_task_struct();
void free_task_struct(unsigned long ptr);

unsigned long get_free_page();
void page_free();

unsigned long vaddr_to_paddr(unsigned long);
unsigned long get_pfn(unsigned long);
void init_page_status();

struct page_struct {
    char in_use;
};

extern struct page_struct page[LAST_PAGE];
#endif

#endif /*_MM_H */
