#include "mm.h"

static unsigned short o_mem_map [ PAGING_PAGES ] = {0,};

unsigned long old_get_free_page()
{
	for (int i = 0; i < PAGING_PAGES; i++){
		if (o_mem_map[i] == 0){
			o_mem_map[i] = 1;
			return LOW_MEMORY + i*PAGE_SIZE;
		}
	}
	return 0;
}

void old_free_page(unsigned long p){
	o_mem_map[(p - LOW_MEMORY) / PAGE_SIZE] = 0;
}