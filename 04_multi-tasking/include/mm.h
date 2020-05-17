#ifndef _MM_H
#define _MM_H
#include "peripherals/base.h"

// TODO: Understand those shift
#define PAGE_SHIFT 12
#define TABLE_SHIFT 9
#define SECTION_SHIFT (PAGE_SHIFT + TABLE_SHIFT)
#define PAGE_SIZE (1 << PAGE_SHIFT)
#define SECTION_SIZE (1 << SECTION_SHIFT)

#define LOW_MEMORY (2 * SECTION_SIZE) // 4MB
#define HIGH_MEMORY PBASE
#define NUM_PAGES 64

#define LOW_KERNEL_STACK LOW_MEMORY
#define HIGH_KERNEL_STACK LOW_MEMORY + NUM_PAGES * PAGE_SIZE
#define LOW_USER_STACK HIGH_KERNEL_STACK

#define PAGING_MEMORY (HIGH_MEMORY - LOW_MEMORY)
// #define PAGING_PAGES (PAGING_MEMORY / PAGE_SIZE)

#ifndef __ASSEMBLER__
void memzero(unsigned long src, unsigned long n);
unsigned long allocate_task_struct();
void free_task_struct(unsigned long ptr);
#endif

#endif /*_MM_H */
