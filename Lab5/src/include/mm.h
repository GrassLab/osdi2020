#ifndef	_MM_H
#define	_MM_H

#include "peripherals/base.h"
#define VA_START 			0xffff000000000000
#define PHYS_MEMORY_SIZE 		0x40000000	

#define PAGE_SHIFT	 	12
#define TABLE_SHIFT 		9
#define SECTION_SHIFT		(PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE   		(1 << PAGE_SHIFT)  //2^12	
#define SECTION_SIZE		(1 << SECTION_SHIFT) //2^21 

#define HIGH_MEMORY             DEVICE_BASE // 0x3F00 0000
#define LOW_MEMORY              (3 * SECTION_SIZE) // 3 * 2^21 = 6M
                                                   // =0x60 0000 
						   
#define PGD_SHIFT			(PAGE_SHIFT + 3*TABLE_SHIFT) //39
#define PUD_SHIFT			(PAGE_SHIFT + 2*TABLE_SHIFT) //30
#define PMD_SHIFT			(PAGE_SHIFT + TABLE_SHIFT)   //21
#define PTE_SHIFT                       (PAGE_SHIFT)                 //12

#define PAGE_ENTRY            (HIGH_MEMORY/PAGE_SIZE)
#define FIRST_AVAILIBLE_PAGE  (LOW_MEMORY/PAGE_SIZE)

// macro for page used state
#define NOT_USED      0
#define USED_NOW      1
#define USED_PRESERVE 2

#ifndef __ASSEMBLER__

struct page_struct{
	int used;
};

extern struct page_struct page[PAGE_ENTRY];

void init_page_struct();
unsigned long virtual_to_pfn(unsigned long vir);
unsigned long virtual_to_physical(unsigned long vir);


unsigned long get_free_page();
unsigned long allocate_kernel_page();

void memzero(unsigned long src, unsigned long n);
void fork_memcpy(void *dest, const void *src, unsigned long len);

void free_page(unsigned long p);
#endif

#endif  /*_MM_H */
