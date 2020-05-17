#ifndef __MM_H__
#define __MM_H__

#pragma once

#define PAGE_SHIFT 12
#define TABLE_SHIFT 9
#define SECTION_SHIFT (PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE (1 << PAGE_SHIFT)
#define SECTION_SIZE (1 << SECTION_SHIFT)

#define LOW_MEMORY (2 * SECTION_SIZE)

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
