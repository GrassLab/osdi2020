#ifndef _MM_H
#define _MM_H

#define VA_START 0xffff000000000000

#define PAGE_MASK			0xfffffffffffff000
#define PAGE_SHIFT	 		12
#define TABLE_SHIFT 			9
#define SECTION_SHIFT			(PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE   			(1 << PAGE_SHIFT)	
#define SECTION_SIZE			(1 << SECTION_SHIFT)	

#define LOW_MEMORY              	(2 * SECTION_SIZE)
#define HIGH_MEMORY 0x3F000000

#define PAGING_MEMORY (HIGH_MEMORY - LOW_MEMORY)
#define PAGING_PAGES (PAGING_MEMORY / PAGE_SIZE)

#define PTRS_PER_TABLE			(1 << TABLE_SHIFT)

#define PGD_SHIFT			PAGE_SHIFT + 3*TABLE_SHIFT
#define PUD_SHIFT			PAGE_SHIFT + 2*TABLE_SHIFT
#define PMD_SHIFT			PAGE_SHIFT + TABLE_SHIFT

#ifndef __ASSEMBLER__

#include "type.h"
#include "task/taskStruct.h"

struct page
{
    bool used;
    uint32_t pfn;
    uint32_t order;
    struct page* prev;
    struct page* next;
};

void memzero(unsigned long src, unsigned long n);
void memcpy(unsigned long dst, unsigned long src, unsigned long n);
void setPgd();

uint64_t allocateKernelPage();
uint64_t allocateUserPage(struct task *task, uint64_t va);

#endif
#endif