#ifndef MEMORY_H
#define MEMORY_H

#define VA_START 			0xffff000000000000
#define PHYS_MEMORY_SIZE 	0x40000000

#define PAGE_SHIFT	 	    12
#define TABLE_SHIFT 		9
#define SECTION_SHIFT		(PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE   		(1 << PAGE_SHIFT)  //2^12	
#define SECTION_SIZE		(1 << SECTION_SHIFT) //2^21 

#define HIGH_MEMORY         0x3F000000         // 0x3F000000
#define LOW_MEMORY          (3 * SECTION_SIZE)  // 3 * 2^21 = 6M
                                                //          = 0x600000 

#define PTRS_PER_TABLE			(1 << TABLE_SHIFT)						   
#define PGD_SHIFT			(PAGE_SHIFT + 3*TABLE_SHIFT) //39
#define PUD_SHIFT			(PAGE_SHIFT + 2*TABLE_SHIFT) //30
#define PMD_SHIFT			(PAGE_SHIFT + TABLE_SHIFT)   //21
#define PTE_SHIFT                       (PAGE_SHIFT)                 //12

#define PAGE_ENTRY            (HIGH_MEMORY / PAGE_SIZE)
#define FIRST_AVAILIBLE_PAGE  (LOW_MEMORY / PAGE_SIZE)

#define PAGE_NOT_USED 0 
#define PAGE_USED 1
#define PAGE_PRESERVE 2

// Lab-6 Page
#define TOTAL_PAGE_NUMBER       2048
#define BUDDY_MAX_ORDER         11
#define MAX_BUDDY_PAGE_NUMBER   (1 << (BUDDY_MAX_ORDER))
#define LOW_PAGE_POOL_MEMORY    0xf00000
#define HIGH_PAGE_POOL_MEMORY   LOW_PAGE_POOL_MEMORY + TOTAL_PAGE_NUMBER * PAGE_SIZE

#define NEXT_BUDDY_START(now,order)	((now)+(1<<(order)))
#define PREV_BUDDY_START(now,order)	((now)-(1<<(order)))
#define BUDDY_END(now,order)	((now)+(1<<(order))-1)

#ifndef __ASSEMBLER__

#include "list.h"

typedef struct page {
    int used;
    int order;
    int index;
    unsigned long physicalAddr;
    List list;
} Page;

Page pagePool[TOTAL_PAGE_NUMBER];
List pageBuddy[BUDDY_MAX_ORDER+1];

void page_sys_init();
void print_buddy();
Page* get_page();
Page* release_redundant_memory(Page *alloc_page, int get_page_order, int req_page_order);
void free_page(Page *page);

#endif
#endif