#ifndef _MM_H
#define _MM_H

#include "list.h"

#define KVA_BASE                0xffff000000000000 //kernel virtual address base
#define UVA_BASE                0x0000000000000000 //user virtual address base

#define PAGE_SHIFT              12
#define TABLE_SHIFT             9
#define BLOCK_SHIFT             (PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE               (1 << PAGE_SHIFT)	
#define BLOCK_SIZE              (1 << BLOCK_SHIFT)	
#define PAGE_TABLE_SIZE         PAGE_SIZE

#define PHY_MEM_BASE            0x00000000
#define LOW_MEMORY              (2 * BLOCK_SIZE)
#define HIGH_MEMORY             0x3f000000
#define LOCAL_DEV_BASE          0x40000000

#define PGD_BASE                PHY_MEM_BASE
#define PUD_BASE                PGD_BASE + PAGE_TABLE_SIZE
#define PMD_BASE                PUD_BASE + PAGE_TABLE_SIZE

#define NR_PAGE                 4096
#define FPN_MASK                0x0000fffffffff000

#define MAX_ORDER               9
#define MAX_ORDER_SIZE          (1 << MAX_ORDER)

#define MAX_ALLOCATOR_NUM       16
// #define MAX_POOL_PAGE           32
#define MIN_ALLOCATOR_SIZE      8//we need to put the pointer of next free space in chunk

#define MIN_KMALLOC_ORDER       3
#define MAX_KMALLOC_ORDER       11
#define MIN_KMALLOC_SIZE        (2 << MIN_KMALLOC_ORDER)
#define MAX_KMALLOC_SIZE        (2 << MAX_KMALLOC_ORDER)




#ifndef __ASSEMBLER__
#define PFN_MASK                    0x0000FFFFFFFFF000
#define PHY_ADDR_TO_PFN(addr)       (((((unsigned long)(addr)) - LOW_MEMORY) & PFN_MASK) >> PAGE_SHIFT)

struct page{
    struct list_head list;
    // buddy
    int order;
    // object allocator
    struct obj_alloc *obj_alloc;
    unsigned int nr_obj;
    int nr_unused;
    void **obj_freelist;
    // general info
    int pfn;
    int used;
    unsigned long phy_addr;
};

struct page bookkeep[NR_PAGE];

#define FIND_BUDDY_PFN(pfn, order)       ((pfn) ^ (1<<(order)))
#define FIND_LBUDDY_PFN(pfn, order)      ((pfn) & (~(1<<(order))))
#define FIND_RBUDDY_PFN(pfn, order)      ((pfn) | (1<<(order)))

struct list_head buddy_freelist[MAX_ORDER + 1];// +1 for order 0

struct obj_alloc{
    struct page* curr_page;//current allocating page
    list_head_t partial;
    list_head_t full;
    list_head_t empty;
    unsigned int objsize;// size 0 for non-registered pool
};

struct obj_alloc allocator_pool[MAX_ALLOCATOR_NUM];

unsigned long get_kstack_base(unsigned long task_id);
unsigned long get_ustack_base(unsigned long task_id);
void mm_init();
int register_obj_allocator(unsigned int objsize);
void *obj_allocate(int token);
void obj_free(void *obj);
void *kmalloc(unsigned int size);
void kfree(void * block);
struct page *__buddy_block_alloc(int order);
void __buddy_block_free(struct page* block);
void __init_obj_page(struct page* page, unsigned size);
#endif //__ASSEMBLER__


#endif//_MM_H */