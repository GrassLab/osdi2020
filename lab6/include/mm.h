#ifndef	_MM_H
#define	_MM_H

#include "list.h"

#define NULL                    ((void *)0) 

#define PAGE_SHIFT	 		    12
#define TABLE_SHIFT 			9
#define SECTION_SHIFT			(PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE   			(1 << PAGE_SHIFT)	
#define SECTION_SIZE			(1 << SECTION_SHIFT)	

#define EL1_LOW_MEMORY          (2 * SECTION_SIZE)
#define EL0_LOW_MEMORY          (4 * SECTION_SIZE)

#define PAGE_SIZE               (1 << PAGE_SHIFT) // 2^12 = 4K

#define HIGH_PAGE_POOL_MEMORY   LOW_PAGE_POOL_MEMORY + TOTAL_PAGE_NUMBER*PAGE_SIZE
#define LOW_PAGE_POOL_MEMORY    0xf00000

#define MAX_ORDER 9

// #define TOTAL_PAGE_NUMBER       ((HIGH_PAGE_POOL_MEMORY-LOW_PAGE_POOL_MEMORY)/PAGE_SIZE)
#define TOTAL_PAGE_NUMBER       2048
#define MAX_BUDDY_PAGE_NUMBER   (1 << (MAX_ORDER-1))

#define NEXT_BUDDY_START(now,order)	((now)+(1<<(order)))
#define PREV_BUDDY_START(now,order)	((now)-(1<<(order)))
#define BUDDY_END(now,order)	((now)+(1<<(order))-1)
#define SLUB_INDEX_TO_SIZE(slub_index)	(1 << (slub_index+3))
#define SLUB_NUMBER             9 // 8,16,32,64...2048

#ifndef __ASSEMBLER__
    void memzero(unsigned long src, unsigned long n);
    void init_buddy_sys();
    void init_page_sys();
    unsigned long give_slab(int size);
    enum {
        NOT_USED,
        USED,
    };

    struct page {
        int used;
        int order;
        int page_index;
        unsigned long phy_addr;
        struct slub* slub_next;
        int slub_num;
        int slub_index;
        list_ptr_t list;
    };

    struct slub {
        struct slub* next;
    };

    struct kmem_cache_cpu{
        struct slub* free_list;
        struct page* page;
        list_ptr_t partial;
    };

    struct kmem_cache{
        struct kmem_cache_cpu cache_cpu;
    };

    typedef struct page page_t;
    typedef struct slub slub_t;
    typedef struct kmem_cache kmem_cache_t;
    typedef struct kmem_cache_cpu kmem_cache_cpu_t;
    page_t page_t_pool[TOTAL_PAGE_NUMBER];
    list_ptr_t page_buddy[MAX_ORDER];
    // slub_t kmem_cache[SLUB_NUMBER];
    kmem_cache_t kmem_cache_arr[SLUB_NUMBER]; 

#endif

#endif  /*_MM_H */