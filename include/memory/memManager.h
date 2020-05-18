#ifndef _MM_H
#define _MM_H

#include "type.h"

#define VA_START 0xffff000000000000

#define PAGE_SIZE 0x200000

#define LOW_MEMORY 0x4000 + 0x400 * 0x1000 + 0x1000
#define HIGH_MEMORY 0x3F000000

#define PAGING_MEMORY (HIGH_MEMORY - LOW_MEMORY)
#define PAGING_PAGES (PAGING_MEMORY / PAGE_SIZE)

struct page
{
    bool used;
};

void memzero(unsigned long src, unsigned long n);
void memcpy(unsigned long dst, unsigned long src, unsigned long n);

#endif