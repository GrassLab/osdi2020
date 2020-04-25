#include "include/mm.h"
#include "include/printf.h"

static unsigned short mem_map [PAGING_PAGES] = {0,};

unsigned long get_free_page()
{
	for (int i = 0; i < PAGING_PAGES; i++){
		// finding availible memory space for your process
		if (mem_map[i] == 0){
			//printf("Using Page: %d\r\n",i);
			mem_map[i] = 1;
			return LOW_MEMORY + i*PAGE_SIZE;
		}
	}
	return 0;
}

void free_page(unsigned long p){
	printf("Free Page %d\r\n",(p - LOW_MEMORY) / PAGE_SIZE);
	mem_map[(p - LOW_MEMORY) / PAGE_SIZE] = 0;
}
 
