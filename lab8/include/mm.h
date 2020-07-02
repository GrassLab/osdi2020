#ifndef _MM_H
#define _MM_H

#include "io.h"

#define VA_START 0xffff000000000000
#define PHYS_MEMORY_SIZE 0x40000000

#define PAGE_INDEX 12
#define TABLE_INDEX 9
#define SECTION_SHIFT (PAGE_INDEX + TABLE_INDEX)

#define PAGE_SIZE (1 << PAGE_INDEX) //2^12
#define SECTION_SIZE (1 << SECTION_SHIFT) //2^21

#define HIGH_MEMORY MMIO_BASE // 0x3F00 0000
#define LOW_MEMORY (3 * SECTION_SIZE) // 3 * 2^21 = 6M =0x60 0000
// TODO: problems
#define PTRS_PER_TABLE (1 << TABLE_INDEX)
#define PGD_SHIFT (PAGE_INDEX + 3 * TABLE_INDEX) //39
#define PUD_SHIFT (PAGE_INDEX + 2 * TABLE_INDEX) //30
#define PMD_SHIFT (PAGE_INDEX + TABLE_INDEX) //21
#define PTE_SHIFT (PAGE_INDEX) //12

#define PAGE_ENTRY (HIGH_MEMORY / PAGE_SIZE)
#define FIRST_AVAILIBLE_PAGE (LOW_MEMORY / PAGE_SIZE)

#ifndef __ASSEMBLER__
#define USR_PAGE_MAX 10
#define KERNEL_PAGE_MAX 10
#define TOTAL_PAGE_NUM 262144 //	Max page number for rpi3 b+

#define USED 1
#define FREE 0

struct page {
	uint8_t used;
};
struct vm_area {
	uint64_t start;
	uint64_t end;
	uint8_t prot; //	vm protection
	struct vm_area *next, *prev; // prepare for linked list
};
struct mm_struct {
	uint64_t *pgd;
	uint16_t user_page_count;
	uint16_t kernel_page_count;
	// struct vm_area *vm_start;
	struct vm_area user_page[USR_PAGE_MAX], kernel_page[KERNEL_PAGE_MAX];
};
extern struct page page_map[TOTAL_PAGE_NUM];

void page_table_init();
void *virt_to_phy(void *virt);
uint64_t phy_to_ppn(void *phy);
uint64_t *page_alloc();
int page_free(void *addr);

#endif

#endif /*_MM_H */