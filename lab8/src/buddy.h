#ifndef _BUDDY_H
#define _BUDDY_H

#define       NULL    ((void *)0)

#define MAX_BUDDY_PAGE_NUM	(9)
#define PAGE_NUM_FOR_MAX_BUDDY	((1<< (MAX_BUDDY_PAGE_NUM - 1))-1)

/*page flags*/
#define PAGE_AVAILABLE		0x00
#define PAGE_DIRTY			0x01
#define PAGE_BUDDY_BUSY		0x02

#define _MEM_START	(LOW_MEMORY + 64 *  PAGE_SIZE)
#define _MEM_END	0x2f000000 

/*MASK is 0xfffff000*/
#define PAGE_MASK	(~(PAGE_SIZE-1))

#define KERNEL_MEM_END	(_MEM_END)
/*the begin and end of the kernel mem which is needed to be paged. 4K alignment*/
#define KERNEL_PAGING_START	((_MEM_START+(~PAGE_MASK))&((PAGE_MASK)))

/* The number of dynamic memory space divided struct page needed*/
#define	KERNEL_PAGING_END	(((KERNEL_MEM_END-KERNEL_PAGING_START)/(PAGE_SIZE+sizeof(struct page)))*(PAGE_SIZE)+KERNEL_PAGING_START)

/*the number of pages*/
#define KERNEL_PAGE_NUM	    ((KERNEL_PAGING_END-KERNEL_PAGING_START)/PAGE_SIZE)

/*the memory space uses for store struct page from KERNEL_PAGE_START to _MEM_END*/
#define KERNEL_PAGE_END	    _MEM_END
#define KERNEL_PAGE_START	(KERNEL_PAGE_END-KERNEL_PAGE_NUM*sizeof(struct page))

/*we can do these all because the page structure that represents one page aera is continuous*/
#define BUDDY_END(x,order)	        ((x)+(1<<(order))-1)
#define NEXT_BUDDY_START(x,order)	((x)+(1<<(order)))
#define PREV_BUDDY_START(x,order)	((x)-(1<<(order)))

unsigned long get_free_page(int order);
void free_page(unsigned long p);
void init_page_map(void);
#endif
