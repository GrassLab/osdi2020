#ifndef	_MM_H
#define	_MM_H

#include "peripherals/base.h"
#include "scheduler.h"

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

#define PTRS_PER_TABLE			(1 << TABLE_SHIFT)						   
#define PGD_SHIFT			(PAGE_SHIFT + 3*TABLE_SHIFT) //39
#define PUD_SHIFT			(PAGE_SHIFT + 2*TABLE_SHIFT) //30
#define PMD_SHIFT			(PAGE_SHIFT + TABLE_SHIFT)   //21
#define PTE_SHIFT                       (PAGE_SHIFT)                 //12

#define PAGE_ENTRY            (HIGH_MEMORY/PAGE_SIZE)
#define FIRST_AVAILIBLE_PAGE  (LOW_MEMORY/PAGE_SIZE)

#define NOT_USED 0 
#define USED_NOW 1
#define PRESERVE 2

#ifndef __ASSEMBLER__

struct page_struct{
	int used;
};

int remain_page;
struct page_struct page[PAGE_ENTRY];

void init_page_struct();
unsigned long virtual_to_physical(unsigned long vir);
unsigned long physical_to_pfn(unsigned long phy);

unsigned long get_free_page();
unsigned long allocate_kernel_page();
unsigned long allocate_user_page(struct task_struct *task,unsigned long vir_addr);

void map_page(struct task_struct *task, unsigned long vir_addr, unsigned long page, unsigned long page_attr);
unsigned long map_table(unsigned long *table, unsigned long shift,unsigned long vir_addr, struct task_struct *task);
void map_entry(unsigned long *pte, unsigned long vir_addr,unsigned long phy_addr,unsigned long page_attr);


void memzero(unsigned long src, unsigned long n);

void memcpy(void *dest, const void *src, unsigned long len);
void free_page(unsigned long p);
void dump_mem(void *src,unsigned long len);
int page_fault_handler(unsigned long addr,unsigned long esr);
int copy_virt_memory(struct task_struct *dst);

void* mmap(void* addr, unsigned long len, int prot, int flags, void* file_start, int file_offset);
#endif

#endif  /*_MM_H */
