#include "mm.h"
static unsigned short mem_map [ PAGING_PAGES ] = {0,};
static unsigned short kernel_stack_map[64] = {0,};

unsigned long get_user_page(int pid)
{
	return LOW_USER_STACK + (pid - 1) * PAGE_SIZE;
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
	return 0;
}

void free_page(unsigned long p){
	mem_map[(p - LOW_MEMORY) / PAGE_SIZE] = 0;
}