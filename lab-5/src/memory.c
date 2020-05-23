#include "memory.h"

int remain_page = PAGE_ENTRY;
Page pages[PAGE_ENTRY];

void paging_init() {
	memset((unsigned long) pages, 0, sizeof(Page) * PAGE_ENTRY);

	unsigned long pfn;	
	pfn = virtual_to_pfn(LOW_MEMORY);
	
	for(int i  =0; i < pfn; i++){
		pages[i].used = PAGE_PRESERVE; 	
	}
}

unsigned long get_free_page() {
	// Start from first availible memory
	// Since some region are used for kernel image / stack
	
	for (int i = FIRST_AVAILIBLE_PAGE; i < PAGE_ENTRY; i++){
		if (pages[i].used == PAGE_NOT_USED){
			pages[i].used = PAGE_USED;

			unsigned long page = i * PAGE_SIZE;
			memzero(VA_START + page, PAGE_SIZE);
			return page;
		}
	}
	return 0;
}

void free_page(unsigned long p) {
	if(pages[p / PAGE_SIZE].used == PAGE_USED)
		pages[p / PAGE_SIZE].used = PAGE_NOT_USED;
}

unsigned long virtual_to_physical(unsigned long vir) {
	unsigned long pfn = (vir << 16) >> 16;
	unsigned long offset = (vir << 52) >> 52;
	pfn = (pfn) >> PTE_SHIFT;
	return pfn*PAGE_SIZE | offset;
}

unsigned long virtual_to_pfn(unsigned long vir){
	// using [47:21] being fpn for 2M section
	unsigned long pfn = (vir<<16)>>16;
	pfn = (pfn)>>PTE_SHIFT;
	return pfn;	
}

unsigned long physical_to_pfn(unsigned long phy){
	return (phy) >> 12;	
}
