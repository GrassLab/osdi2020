#include "memory.h"

int remain_page = PAGE_ENTRY;
Page pages[PAGE_ENTRY];

void paging_init() {
    int i = 0;	
	for(;i < FIRST_AVAILIBLE_PAGE; i++){
		pages[i].used = PAGE_PRESERVE;
		remain_page--;
	}
		
	for(;i < PAGE_ENTRY; i++){
		pages[i].used = PAGE_NOT_USED;
	}
}

unsigned long get_free_page() {

}


unsigned long virtual_to_physical(unsigned long vir) {
	unsigned long pfn = (vir << 16) >> 16;
	unsigned long offset = (vir << 52) >> 52;
	pfn = (pfn) >> PTE_SHIFT;

	return pfn*PAGE_SIZE | offset;
}

unsigned long physical_to_pfn(unsigned long phy){
	return (phy) >> 12;	
}