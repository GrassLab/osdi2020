#ifndef	_MM_H
#define	_MM_H

#include "list.h"

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

#ifndef __ASSEMBLER__
    void memzero(unsigned long src, unsigned long n);
    void init_buddy_sys();
    void init_page_sys();
    enum {
        NOT_USED,
        USED,
    };

    struct page {
        int used;
        int order;
        int page_index;
        unsigned long phy_addr;
        struct list_pointer list;
    };
    typedef struct page page_t;
    page_t page_t_pool[TOTAL_PAGE_NUMBER];
    struct list_pointer page_buddy[MAX_ORDER];

#endif

#endif  /*_MM_H */