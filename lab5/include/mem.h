#pragma once


#define PBASE               0x3F000000

#define PAGE_SHIFT          12
#define TABLE_SHIFT         9
#define SECTION_SHIFT       (PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE           (1 << PAGE_SHIFT)
#define SECTION_SIZE        (1 << SECTION_SHIFT)

#define LOW_MEMORY          (2 * SECTION_SIZE)
#define HIGH_MEMORY         PBASE

#define PAGING_MEMORY       (HIGH_MEMORY - LOW_MEMORY)
#define PAGING_PAGES        (PAGING_MEMORY/PAGE_SIZE)


#ifndef __ASSEMBLER__

unsigned long get_free_page();
unsigned long get_user_free_page();
void free_page(unsigned long p);
void memzero(unsigned long src, unsigned long n);

#endif
