#ifndef MEMORY_H
#define MEMORY_H

#define VA_START 			0xffff000000000000
#define PHYS_MEMORY_SIZE 	0x40000000

#define PAGE_SHIFT	 	    12
#define TABLE_SHIFT 		9
#define SECTION_SHIFT		(PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE   		(1 << PAGE_SHIFT)  //2^12	
#define SECTION_SIZE		(1 << SECTION_SHIFT) //2^21 

#define EL1_LOW_MEMORY              (2 * SECTION_SIZE)
#define EL0_LOW_MEMORY              (4 * SECTION_SIZE)

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

#ifndef __ASSEMBLER__

typedef struct page {
    int used;
} Page;

void paging_init();
unsigned long get_free_page();
void free_page(unsigned long p);
unsigned long virtual_to_physical(unsigned long);
unsigned long physical_to_pfn(unsigned long);
unsigned long virtual_to_pfn(unsigned long);

#endif

#endif