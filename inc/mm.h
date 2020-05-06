#ifndef	_MM_H

#define PAGE_SHIFT	 		    12
#define TABLE_SHIFT 			9
#define SECTION_SHIFT			(PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE   			(1 << PAGE_SHIFT)	
#define SECTION_SIZE			(1 << SECTION_SHIFT)	

#define EL1_LOW_MEMORY          (2 * SECTION_SIZE)
#define EL0_LOW_MEMORY          (4 * SECTION_SIZE)

#define STACK_OFFSET            EL0_LOW_MEMORY - EL1_LOW_MEMORY


#ifndef __ASSEMBLER__
unsigned long get_kstack_base(unsigned long task_id);
void free_kstack(unsigned long p);
extern void memncpy(char *source, char *dest, int n);
extern void memzero(char *dest, int n);
#endif //__ASSEMBLER__

#endif//_MM_H */