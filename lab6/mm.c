#include "mm.h"
#include "uart.h"

// static unsigned short mem_map [PAGING_PAGES] = {0,};
static free_area_t free_block[32] = {
    { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
    { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
    { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
    { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
    { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
    { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
    { 0, 0 }, { 0, 0 }
};

void print_memory_pool()
{
    uart_puts("==========MEMORY POOL==========\r\n");
    for (int i = 0; i < 32; i++) {
        free_area_t* p = &free_block[i];
        p = p->next;
        if (p) {
            uart_puts("Slot ");
            uart_print_int(i);
            uart_puts("\r\n");
            for (int j = 0; p != 0; j++) {
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

void init_memory()
{
    uart_puts("INIT MEMORY\r\n");
    int page_n = PAGING_PAGES;
    uart_puts("TOTAL: ");
    uart_print_int(page_n);
    uart_puts(" pages\r\n");
    free_area_t* free_area_t_p;
    unsigned int base = LOW_MEMORY;
    uart_puts("BASE: 0x");
    uart_hex(base);
    uart_puts("\r\n");

    for (int i = 31; i >= 0; i--) {
        if ((page_n & (0x80000000)) == 0x80000000) {
            free_area_t_p = &free_block[i];
            while (free_area_t_p->next != 0) {
                free_area_t_p = free_area_t_p->next;
            }

            // init next free_area
            free_area_t* tmp = base + sizeof(free_area_t);
            tmp->next = 0;
            tmp->map = base;
            free_area_t_p->next = tmp;
            base += (0x1 << i) * PAGE_SIZE;
        }
        page_n = (page_n << 1);
    }
    print_memory_pool();
}

void remove_from_slot(free_area_t* p, int slot)
{
    free_area_t* tmp = &free_block[slot];
    while (tmp->next != p && tmp->next != 0) {
        tmp = tmp->next;
    }
    if (tmp->next == p)
        tmp->next = tmp->next->next;
}

unsigned int merge_left_block(free_area_t* p, unsigned int addr, int slot)
{
    uart_puts("==========MERGE LEFT BLOCK==========\r\n");
    free_area_t* target = p->next;
    remove_from_slot(target, slot);

    unsigned int block_size = (0x1 << slot) * PAGE_SIZE;

    uart_puts("merge block: 0x");
    uart_hex(target->map);
    uart_puts(" and 0x");
    uart_hex(addr);
    uart_puts(", block size 0x");
    uart_hex(block_size);
    uart_puts(", merge to slot ");
    uart_hex(slot + 1);
    uart_puts("\r\n");

    free_area_t* tmp_addr = (unsigned int)addr + sizeof(free_area_t);
    remove_from_slot(tmp_addr, slot);

    free_area_t* tmp = (unsigned int)target->map + sizeof(free_area_t);
    tmp->next = 0;
    tmp->map = (unsigned int)target->map;

    free_area_t* slot_p = &free_block[slot + 1];
    while (slot_p->next != 0) {
        slot_p = slot_p->next;
    }
    slot_p->next = tmp;

    return tmp->map;
}

unsigned int merge_right_block(free_area_t* p, unsigned int addr, int slot)
{
    uart_puts("==========MERGE RIGHT BLOCK==========\r\n");
    free_area_t* target = p->next;
    remove_from_slot(target, slot);

    unsigned int block_size = (0x1 << slot) * PAGE_SIZE;

    uart_puts("merge block: 0x");
    uart_hex(target->map);
    uart_puts(" and 0x");
    uart_hex(addr);
    uart_puts(", block size 0x");
    uart_hex(block_size);
    uart_puts(", merge to slot ");
    uart_hex(slot + 1);
    uart_puts("\r\n");

    free_area_t* tmp = (unsigned int)addr + sizeof(free_area_t);
    remove_from_slot(tmp, slot);
    tmp->next = 0;
    tmp->map = (unsigned int)addr;

    free_area_t* slot_p = &free_block[slot + 1];
    while (slot_p->next != 0) {
        slot_p = slot_p->next;
    }
    slot_p->next = tmp;

    return tmp->map;
}

int merge_block(unsigned int addr, int slot)
{
    // uart_puts("==========TRY TO MERGE==========\r\n");
    free_area_t* free_area_t_p = &free_block[slot];
    unsigned int tmp_addr;
    unsigned int base = addr;
    int merge = 0;
    unsigned int block_size = (0x1 << slot) * PAGE_SIZE;

    while (free_area_t_p->next != 0) {
        unsigned int next_addr = free_area_t_p->next->map;
        if (next_addr + block_size == base) {
            tmp_addr = merge_left_block(free_area_t_p, base, slot);
            merge = 1;
        } else if (next_addr - block_size == base) {
            tmp_addr = merge_right_block(free_area_t_p, base, slot);
            merge = 1;
        }

        if (merge == 1) {
            break;
        }
        free_area_t_p = free_area_t_p->next;
    }
    // print_memory_pool();
    if (merge == 1) {
        merge_block((unsigned int)tmp_addr, slot + 1);
    }

    return merge;
}

void recycle(unsigned int addr, unsigned int page_n)
{
    free_area_t* free_area_t_p;
    unsigned int base = addr;
    unsigned int page_n_tmp = page_n;

    uart_puts("recycle address: 0x");
    uart_hex(addr);
    uart_puts(" pages: ");
    uart_print_int(page_n);
    uart_puts("\r\n");

    for (int i = 0; i < 32; i++) {
        if ((page_n_tmp & (0x1)) == 0x1) {
            free_area_t_p = &free_block[i];

            int merge = 0;
            merge = merge_block(addr, i);

            if (merge == 0) {
                uart_puts("insert block...");
                uart_print_int(page_n_tmp);
                uart_puts("\r\n");

                // init next free_area
                free_area_t* tmp = base + sizeof(free_area_t);
                tmp->next = free_area_t_p->next;
                tmp->map = (unsigned int)base;
                free_area_t_p->next = tmp;
                base += (unsigned int)((0x1 << i) * PAGE_SIZE);
            }
        }
        page_n_tmp = (page_n_tmp >> 1);
    }
    print_memory_pool();
}

free_area_t* allocate_page(free_area_t* p, int i, unsigned int page_n)
{
    uart_puts("Allocate at slot: ");
    uart_print_int(i);
    free_area_t* target = p->next;
    p->next = target->next;
    remove_from_slot(target, i);
    target->next = 0;

    unsigned int block_size = (unsigned int)((0x1 << i));
    uart_puts(", pages in the block: ");
    uart_print_int(block_size);
    uart_puts(", need page: ");
    uart_print_int(page_n);
    uart_puts(", block at: 0x");
    uart_hex(target->map);
    uart_puts("\r\n");
    int remain = block_size - page_n;
    uart_puts("remain page: ");
    uart_print_int(remain);
    uart_puts("\r\n");

    unsigned int b = target->map;
    unsigned int s = page_n * PAGE_SIZE;
    unsigned int t = s + b;
    if (remain > 0) {
        recycle(t, (remain));
    }
    return target;
}

void* malloc(unsigned int size)
{
    size += sizeof(int);
    uart_puts("==========MALLOC==========\r\n");
    uart_puts("malloc need ");
    // first word to record number of allocated pages
    unsigned int page_n = size / PAGE_SIZE;
    if (size % PAGE_SIZE != 0) {
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
    for (int i = 31; i >= 0; i--) {
        if ((tmp_page_n & (0x80000000)) == 0x80000000) {
            int start = i;
            if (((tmp_page_n << 1) & 0xffffffff) != 0x0) {
                uart_puts("extend...\r\n");
                start++;
            }
            uart_print_int(start);
            for (int j = start; j < 32; j++) {
                free_area_t* p = &free_block[j];
                if (p->next != 0) {
                    p = allocate_page(p, j, page_n);
                    uart_puts("[Allocate at 0x");
                    uart_hex((unsigned int)(p->map));
                    uart_puts("]\r\n");
                    int* memory_size = p->map;

                    *memory_size = page_n;

                    return (unsigned int)(p->map) + sizeof(int);
                }
            }
        }
        tmp_page_n = (tmp_page_n << 1);
    }
    uart_puts("malloc failed.\r\n");
    return 0;
}

void free_memory(unsigned long addr)
{
    uart_puts("==========FREE==========\r\n");
    addr -= sizeof(int);
    int* memory_size = (int*)addr;
    uart_puts("free at 0x");
    uart_hex(addr);
    uart_puts(" with ");
    uart_print_int(*memory_size);
    uart_puts(" pages\r\n");

    unsigned int pages = *memory_size;
    unsigned int accu = 0;
    for (int i = 0; i < 31; i++) {
        if ((pages & 0x1) == 0x1) {
            unsigned tmp_addr = addr + (*memory_size) * PAGE_SIZE;
            tmp_addr = tmp_addr - (0x1 << (i)) * PAGE_SIZE - accu;
            accu += (0x1 << (i)) * PAGE_SIZE;
            recycle(tmp_addr, (0x1 << i));
        }
        pages = pages >> 1;
    }
    // print_memory_pool();
}

unsigned int variable_base = VARIABLE_LOW_MEMORY;
static int mem_map[36] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0
};

void* variable_malloc(unsigned int size)
{
    uart_puts("==========VARIABLE MALLOC==========\r\n");
    size += sizeof(int);
    uart_puts("malloc need ");
    uart_print_int(size);
    uart_puts(" bytes\r\n");

    for (int i = 0; i < 9; i++) {
        if ((0x1 << i) < size)
            continue;
        for (int j = 0; j < 4; j++) {
            if (mem_map[i * 4 + j] == 0) {
                mem_map[i * 4 + j] = 1;
                unsigned int ret_addr = 0;
                for (int k = 0; k < i - 1; k++) {
                    for (int m = 0; m < 4; m++) {
                        ret_addr += (0x1 << k);
                    }
                }
                for (int k = 0; k < j; k++) {
                    ret_addr += (0x1 << i);
                }
                int* stmp = VARIABLE_LOW_MEMORY + ret_addr;
                *stmp = i * 4 + j;

                uart_puts("[Allocate at 0x");
                uart_hex((unsigned int)(VARIABLE_LOW_MEMORY + ret_addr + sizeof(int)));
                uart_puts("]\r\n");

                return VARIABLE_LOW_MEMORY + ret_addr + sizeof(int);
            }
        }
    }
    uart_puts("malloc failed.\r\n");
    return 0;
}

void print_variable_memory_pool()
{
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 4; j++) {
            uart_print_int(mem_map[i * 4 + j]);
            uart_puts(" ");
        }
        uart_puts("\r\n");
    }
}

void variable_free(unsigned int addr)
{
    uart_puts("==========VARIABLE FREE==========\r\n");
    uart_puts("free at 0x");
    uart_hex(addr);
    uart_puts("\r\n");
    addr -= sizeof(int);
    int* stamp = addr;
    mem_map[*stamp] = 0;
    // print_variable_memory_pool();
}