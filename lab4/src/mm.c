#include "mm.h"
static unsigned short mem_map [ PAGING_PAGES ] = {0,};
static unsigned short kernel_stack_map[64] = {0,};
unsigned long get_free_page()
{
	for (int i = 0; i < PAGING_PAGES; i++){
		if (mem_map[i] == 0){
			mem_map[i] = 1;
			return LOW_MEMORY + i*PAGE_SIZE;
		}
	}
	return 0;
}

unsigned long get_kernel_id_page()
{
	unsigned long i;
	for (i = 0 ; i < 64; i++) {
		if (kernel_stack_map[i] == 0) {
			kernel_stack_map[i] = 1;
			return i;
		}
	}
}

void free_page(unsigned long p){
	mem_map[(p - LOW_MEMORY) / PAGE_SIZE] = 0;
}