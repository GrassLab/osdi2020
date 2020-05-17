#ifndef __MM_H__
#define __MM_H__

#pragma once

/* mapping level */
/* 0 for 2 lv translation */
/* 1 for 3 lv translation */
/* 2 for 4 lv translation */
#define MLV 2

#define PAGE_SHIFT 12
#define TABLE_SHIFT 9
#define SECTION_SHIFT (PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE (1 << PAGE_SHIFT)
#define SECTION_SIZE (1 << SECTION_SHIFT)

#define LOW_MEMORY (2 * SECTION_SIZE)

/* refer */

#define PG_DIR_SIZE			(3 * PAGE_SIZE)
#define PHYS_MEMORY_SIZE 		0x40000000
#define VA_START  0xffff000000000000
#define PTRS_PER_TABLE			(1 << TABLE_SHIFT)
#define PGD_SHIFT			PAGE_SHIFT + 3*TABLE_SHIFT
#define PUD_SHIFT			PAGE_SHIFT + 2*TABLE_SHIFT
#define PMD_SHIFT			PAGE_SHIFT + TABLE_SHIFT



#ifndef __ASSEMBLER__
void memzero(unsigned long src, unsigned long n);

#define PAGE_NUM 1024 /* 2mb per page */
#define NOT_USED 0
#define USED_NOW 1
#define PRESERVE 2

typedef struct page_tag {
  int status;
} Page;

#endif

#endif
