#include "include/mm.h"
#include "include/uart.h"

static unsigned short mem_map [PAGING_PAGES] = {0,};

unsigned long get_free_page()
{
	for (int i = 0; i < PAGING_PAGES; i++){
		// finding availible memory space for your process
		if (mem_map[i] == 0){
			//uart_hex(i);
			//uart_send_string(" Using\r\n");
			mem_map[i] = 1;
			return LOW_MEMORY + i*PAGE_SIZE;
		}
	}
	return 0;
}

void free_page(unsigned long p){
	mem_map[(p - LOW_MEMORY) / PAGE_SIZE] = 0;
	//uart_hex((p - LOW_MEMORY) / PAGE_SIZE);
	//uart_send_string("Release\r\n");
}
