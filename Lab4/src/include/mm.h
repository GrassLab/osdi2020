#ifndef	_MM_H
#define	_MM_H

#include "peripherals/base.h"

#define PAGE_SHIFT	 	12
#define TABLE_SHIFT 		9
#define SECTION_SHIFT		(PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE   		(1 << PAGE_SHIFT)  //2^12	
#define SECTION_SIZE		(1 << SECTION_SHIFT) //2^21 

#define HIGH_MEMORY             PBASE // 0x3F00 0000
#define LOW_MEMORY              (2 * SECTION_SIZE) //2^22 = 4M 
						   //= 0x40 0000

#define PAGING_MEMORY           (HIGH_MEMORY - LOW_MEMORY)
#define PAGING_PAGES            (PAGING_MEMORY/PAGE_SIZE) 

#ifndef __ASSEMBLER__
unsigned long get_free_page();
void memzero(unsigned long src, unsigned long n);
void free_page(unsigned long p);
#endif

#endif  /*_MM_H */
