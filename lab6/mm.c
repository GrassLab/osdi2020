#include "uart.h"
#include "mm.h"

static unsigned short mem_map [PAGING_PAGES] = {0,};
static free_area_t free_block[32];

void init_memory() {
    int page_n = PAGING_PAGES;
    uart_puts("Total: ");
    uart_print_int(page_n);
    uart_puts(" pages\r\n");
    free_area_t *free_area_t_p;
    unsigned int base = LOW_MEMORY;

    for(int i=31; i>=0; i--) {
        if((page_n & (0x80000000)) == 0x80000000) {
            free_area_t_p = &free_block[i];
            while(free_area_t_p->next != 0) {
                free_area_t_p = free_area_t_p->next;
            }
            free_area_t_p->next = base + (0x1 << i);

            // init next free_area
            free_area_t *tmp = base + (0x1 << i) - sizeof(free_area_t);
            tmp->next = 0;
            tmp->map = 0;
            free_area_t_p->map = base + (0x1 << i);
            base += (0x1 << i);
        }
        page_n = (page_n << 1);
    }
}

void recycle(unsigned int addr, unsigned int page_n) {
    free_area_t *free_area_t_p;
    unsigned int base = addr;

    // uart_puts("remain pages: ");
    // uart_hex(page_n);
    // uart_puts("\r\n");

    for(int i=31; i>=0; i--) {
        // uart_puts("At: ");
        // uart_print_int(i);
        // uart_puts("\r\n");
        if((page_n & (0x80000000)) == 0x80000000) {
            free_area_t_p = &free_block[i];
            while(free_area_t_p->next != 0) {
                free_area_t_p = free_area_t_p->next;
            }
            free_area_t_p->next = base + (0x1 << i);
            // init next free_area
            free_area_t *tmp = base + (0x1 << i) - sizeof(free_area_t);
            tmp->next = 0;
            tmp->map = 0;
            free_area_t_p->map = base + (0x1 << i);
            base += (0x1 << i);
        }
        page_n = (page_n << 1);
    }
}

void allocate_page(free_area_t *p, int i, unsigned int page_n) {
    free_area_t *target = p->next;
    p->next = target->next;
    target->next = 0;

    unsigned int block_size = (unsigned int)(0x1 << i);
    // uart_puts("block size: ");
    // uart_hex(block_size);
    // uart_puts("\r\n");
    unsigned int remain = block_size - page_n;
    // uart_puts("remain size: ");
    // uart_hex(remain);
    // uart_puts("\r\n");
    recycle(p->map + block_size - remain, remain);
}

void *malloc(unsigned int size)
{
    uart_puts("========================\r\n");
    uart_puts("malloc need ");
    // first word to record number of allocated pages
    unsigned int page_n = size / PAGE_SIZE;
    if(size%PAGE_SIZE != 0) {
        page_n++;
    }
    uart_print_int(page_n);
    uart_puts(" pages\r\n");

    // resize to align 4KB
    size = (0x1 << (page_n-1)) << PAGE_SHIFT;
    uart_puts("Allocated ");
    uart_print_int(size);
    uart_puts(" Bytes\r\n");
    
    int tmp_page_n = page_n;
	for(int i=31; i>=0; i--) {
        // uart_puts("0x");
        // uart_hex(page_n);
        // uart_puts("\r\n");
        if((tmp_page_n & (0x80000000)) == 0x80000000) {
            // uart_print_int(i);
            // uart_puts(" match\r\n");
            for(int j=i; j<32; j++) {
                free_area_t *p = &free_block[j];
                if(p->next != 0) {
                    allocate_page(p, j, page_n);
                    uart_puts("Allocate at 0x");
                    uart_hex(((unsigned int)(p->map) << PAGE_SHIFT));
                    uart_puts("\r\n");
                    return p->map;
                }
            }
        }
        tmp_page_n = (tmp_page_n << 1);
    }
    uart_puts("malloc failed.\r\n");
	return 0;
}

void free_page(unsigned long p){
	mem_map[(p - LOW_MEMORY) / PAGE_SIZE] = 0;
}