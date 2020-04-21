#ifndef	_MM_H
#define	_MM_H

#define PAGE_SHIFT				12	// 2^12 bytes
#define TABLE_SHIFT				9	// 2^9 bytes
#define SECTION_SHIFT			(PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE				(1 << PAGE_SHIFT)	
#define SECTION_SIZE			(1 << SECTION_SHIFT)	

#define LOW_MEMORY				(2 * SECTION_SIZE)	// 2 * (2^21) bytes ---> 4MB

#ifndef __ASSEMBLER__
void memzero(unsigned long src, unsigned long n);
#endif

#endif  /*_MM_H */
