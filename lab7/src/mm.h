#pragma once

#ifndef	_MM_H
#define	_MM_H

#define VA_START  0xffff000000000000

#define PBASE 0x3F000000
#define PAGE_SHIFT 12
#define TABLE_SHIFT 9
#define SECTION_SHIFT (PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE (1 << PAGE_SHIFT)
#define SECTION_SIZE (1 << SECTION_SHIFT)

#define LOW_MEMORY (2 * SECTION_SIZE)
#define HIGH_MEMORY PBASE

#define PAGING_MEMORY (HIGH_MEMORY - LOW_MEMORY)
#define PAGING_PAGES (PAGING_MEMORY / PAGE_SIZE)

#define THREAD_SIZE PAGE_SIZE


void memzero(unsigned long src, unsigned long n);
void memcpy(unsigned long src, unsigned long dst, unsigned long n);

static unsigned short mem_map[PAGING_PAGES] = { 0, };

unsigned long get_free_page();
void free_page(unsigned long p);

#endif  /*_MM_H */