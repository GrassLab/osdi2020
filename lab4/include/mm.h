#ifndef	_MM_H
#define	_MM_H
#include "peripherals/base.h"

#define PAGE_SHIFT	 		    12
#define TABLE_SHIFT 			9
#define SECTION_SHIFT			(PAGE_SHIFT + TABLE_SHIFT)
#define PAGE_SIZE   		    (1 << PAGE_SHIFT)	
#define SECTION_SIZE		    (1 << SECTION_SHIFT)	

#define PAGE_SIZE   			(1 << PAGE_SHIFT)	
#define SECTION_SIZE			(1 << SECTION_SHIFT)	

#define LOW_MEMORY              SECTION_SIZE  // warning 0x200000
#define HIGH_MEMORY             PBASE

#define LOW_KERNEL_STACK        LOW_MEMORY
#define HIGH_KERNEL_STACK       LOW_MEMORY + 64 * PAGE_SIZE
#define LOW_USER_STACK          HIGH_KERNEL_STACK

#define PAGING_MEMORY           (HIGH_MEMORY - LOW_MEMORY)
#define PAGING_PAGES            (PAGING_MEMORY/PAGE_SIZE)

#ifndef __ASSEMBLER__
unsigned long get_kernel_id_page();
unsigned long get_user_page(int pid);
void free_page(unsigned long p);
void memzero(unsigned long src, unsigned long n);
#endif

#endif  /*_MM_H */
