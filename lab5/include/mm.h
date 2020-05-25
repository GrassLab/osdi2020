#ifndef	_MM_H
#define	_MM_H

#define VA_START  0xffff000000000000
#define DEVICE_BASE 0x3F000000
#define PBASE   (VA_START + DEVICE_BASE)
#define PHYS_MEMORY_SIZE 		0x40000000

#define PAGE_SHIFT	 		12
#define TABLE_SHIFT 			9
#define SECTION_SHIFT			(PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE   			(1 << PAGE_SHIFT)	
#define SECTION_SIZE			(1 << SECTION_SHIFT)	

#define LOW_MEMORY              (3*SECTION_SIZE)
#define HIGH_MEMORY             DEVICE_BASE

#define PTRS_PER_TABLE			(1 << TABLE_SHIFT)						   
#define PGD_SHIFT			(PAGE_SHIFT + 3*TABLE_SHIFT) //39
#define PUD_SHIFT			(PAGE_SHIFT + 2*TABLE_SHIFT) //30
#define PMD_SHIFT			(PAGE_SHIFT + TABLE_SHIFT)   //21
#define PTE_SHIFT                       (PAGE_SHIFT)                 //12

#define LOW_KERNEL_STACK        LOW_MEMORY
#define HIGH_KERNEL_STACK       LOW_MEMORY + 64 * PAGE_SIZE
#define LOW_USER_STACK          HIGH_KERNEL_STACK

#define PAGING_MEMORY           (HIGH_MEMORY - LOW_MEMORY)
#define PAGING_PAGES            (PAGING_MEMORY / PAGE_SIZE)

#define PAGE_ENTRY            (HIGH_MEMORY/PAGE_SIZE)
#define FIRST_AVAILIBLE_PAGE  (LOW_MEMORY/PAGE_SIZE)

#define NOT_USED 0 
#define USING    1
#define PRESERVE 2



#ifndef __ASSEMBLER__

struct page_struct {
    int used;
};


struct page_struct page[PAGE_ENTRY];

unsigned long get_free_page();
void free_page(unsigned long p);
void memzero(unsigned long src, unsigned long n);

unsigned long get_user_page(int pid);
unsigned long get_free_page_id();

#endif

#endif  /*_MM_H */