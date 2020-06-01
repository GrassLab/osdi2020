#ifndef _MM_H
#define _MM_H
#include "list.h"

#define PAGE_SHIFT              12
#define TABLE_SHIFT             9
#define SECTION_SHIFT           (PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE               (1 << PAGE_SHIFT)	
#define SECTION_SIZE            (1 << SECTION_SHIFT)	

#define HIGH_MEMORY             0x3f000000
#define LOW_MEMORY              (2 * SECTION_SIZE)

#define STACK_OFFSET            EL0_LOW_MEMORY - EL1_LOW_MEMORY

#define NR_PAGE                 512//4096

#define MAX_ORDER               9
#define MAX_ORDER_SIZE          (1 << MAX_ORDER)

#ifndef __ASSEMBLER__
struct page{
    struct list_head list;
    int order;
    int pfn;
    int used;
    unsigned long phy_addr;
};

struct page bookkeep[NR_PAGE];

#define FIND_BUDDY_PFN(pfn, order)       ((pfn) ^ (1<<(order)))
#define FIND_LBUDDY_PFN(pfn, order)      ((pfn) & (~(1<<(order))))
#define FIND_RBUDDY_PFN(pfn, order)      ((pfn) | (1<<(order)))

struct list_head free_list[MAX_ORDER + 1];// +1 for order 0

unsigned long get_kstack_base(unsigned long task_id);
unsigned long get_ustack_base(unsigned long task_id);
void mm_init();
struct page *__buddy_block_alloc();
void __buddy_block_free(struct page* block);
#endif //__ASSEMBLER__

#endif//_MM_H */