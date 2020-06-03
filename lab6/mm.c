#include "uart.h"
#include "mm.h"

static unsigned short mem_map [PAGING_PAGES] = {0,};
static free_area_t free_block[32] = {
    {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {0, 0}, {0, 0}
};

void print_memory_pool() {
    for(int i=0; i<32; i++) {
        free_area_t *p = &free_block[i];
        p = p->next;
        if(p) {
            uart_puts("Slot ");
            uart_print_int(i);
            uart_puts("\r\n");
            for(int j=0; p != 0; j++) {
                uart_puts("Block ");
                uart_print_int(j);
                uart_puts(" addr: 0x");
                uart_hex((unsigned int)p->map);
                uart_puts("\r\n");
                p = p->next;
            }
        }
    }
}

void init_memory() {
    uart_puts("INIT MEMORY\r\n");
    int page_n = PAGING_PAGES;
    uart_puts("TOTAL: ");
    uart_print_int(page_n);
    uart_puts(" pages\r\n");
    free_area_t *free_area_t_p;
    unsigned int base = LOW_MEMORY;
    uart_puts("BASE: 0x");
    uart_hex(base);
    uart_puts("\r\n");

    for(int i=31; i>=0; i--) {
        if((page_n & (0x80000000)) == 0x80000000) {
            free_area_t_p = &free_block[i];
            while(free_area_t_p->next != 0) {
                free_area_t_p = free_area_t_p->next;
            }

            // init next free_area
            free_area_t *tmp = base + ((0x1 << i)*PAGE_SIZE) - sizeof(free_area_t);
            tmp->next = 0;
            tmp->map = base;
            free_area_t_p->next = tmp;
            base += (0x1 << i)*PAGE_SIZE;
        }
        page_n = (page_n << 1);
    }
    print_memory_pool();
}

void recycle(unsigned int addr, unsigned int page_n) {
    free_area_t *free_area_t_p;
    unsigned int base = addr;

    uart_puts("recycle address: 0x");
    uart_hex(addr);
    uart_puts(" pages: ");
    uart_print_int(page_n);
    uart_puts("\r\n");

    for(int i=31; i>=0; i--) {
        // uart_puts("At: ");
        // uart_print_int(i);
        // uart_puts("\r\n");
        if((page_n & (0x80000000)) == 0x80000000) {
            free_area_t_p = &free_block[i];
            while(free_area_t_p->next != 0) {
                free_area_t_p = free_area_t_p->next;
            }
            
            // init next free_area
            free_area_t *tmp = base + (i*PAGE_SIZE) - sizeof(free_area_t);
            tmp->next = 0;
            tmp->map = 0;
            free_area_t_p->next = tmp;
            free_area_t_p->map = base + (i*PAGE_SIZE);
            base += (i*PAGE_SIZE);
        }
        page_n = (page_n << 1);
    }
}

void allocate_page(free_area_t *p, int i, unsigned int page_n) {
    free_area_t *target = p->next;
    p->next = target->next;
    target->next = 0;

    unsigned int block_size = (unsigned int)((0x1 << i));
    uart_puts("pages in the block: ");
    uart_print_int(block_size);
    uart_puts(" need page: ");
    uart_print_int(page_n);
    uart_puts("\r\n");
    unsigned int remain = block_size - page_n;
    uart_puts("remain page: ");
    uart_print_int(remain);
    uart_puts("\r\n");
    recycle((unsigned int)(p->map) + (page_n << PAGE_SHIFT), (remain));
}

void *malloc(unsigned int size)
{
    size += sizeof(int);
    uart_puts("========================\r\n");
    uart_puts("malloc need ");
    // first word to record number of allocated pages
    unsigned int page_n = size / PAGE_SIZE;
    if(size%PAGE_SIZE != 0) {
        page_n++;
    }
    uart_print_int(size);
    uart_puts(" bytes ");
    uart_print_int(page_n);
    uart_puts(" pages\r\n");

    // resize to align 4KB  
    size = (1 << PAGE_SHIFT) * page_n;
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
                    uart_puts("[Allocate at 0x");
                    uart_hex((unsigned int)(p->map));
                    uart_puts("]\r\n");
                    int *memory_size = p->map;
                    
                    *memory_size = page_n;
                    print_memory_pool();
                    return (unsigned int)(p->map) + sizeof(int);
                }
            }
        }
        tmp_page_n = (tmp_page_n << 1);
    }
    uart_puts("malloc failed.\r\n");
	return 0;
}

void free_memory(unsigned long addr){
    uart_puts("************************\r\n");
    addr -= sizeof(int);
    int *memory_size = (int*)addr;
    uart_puts("free at 0x");
    uart_hex(addr);
    uart_puts(" with ");
    uart_print_int(*memory_size);
    uart_puts(" pages\r\n");

    recycle(addr, (*memory_size));
    uart_puts("************************\r\n");
}