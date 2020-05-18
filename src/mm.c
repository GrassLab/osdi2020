#include "mm.h"
#include "peripherals/mmio.h"
#include "uart0.h"

struct page_t page[PAGE_FRAMES_NUM];
int first_aval_page, last_aval_page;

void mm_init() {
    extern uint8_t __kernel_end; // defined in linker
    uint8_t* kernel_end = &__kernel_end;
    int kernel_end_page = (uint64_t)kernel_end / PAGE_SIZE;
    int mmio_base_page = MMIO_PHYSICAL / PAGE_SIZE;

    int i = 0;
    for (; i < kernel_end_page; i++){
        page[i].used = USED;
    }
    for (; i < mmio_base_page; i++) {
        page[i].used = AVAL;
    }
    for (; i < PAGE_FRAMES_NUM; i++) {
        page[i].used = USED;
    }

    first_aval_page = kernel_end_page + 1;
    last_aval_page = mmio_base_page - 1;
}

uint64_t virtual_to_physical(uint64_t virt_addr) {
    return (virt_addr << 16) >> 16;
}

uint64_t phy_to_pfn(uint64_t phy_addr) {
    return phy_addr >> 12;
}

void memzero(uint8_t* addr, int size) {
    for (int i = 0; i < size; i++) {
        *(addr + i) = 0;
    }
}

uint64_t get_free_page() {
    for (int i = first_aval_page; i < last_aval_page; i++) {
        if (page[i].used == AVAL) {
            page[i].used = USED;
            uint64_t page_addr = i * PAGE_SIZE + KERNEL_VIRT_BASE;
            memzero((uint8_t*)page_addr, PAGE_SIZE);
            return page_addr;
        }
    }
    return 0;
}

void* page_alloc() {
    uint64_t addr = get_free_page();
    if (addr == 0) {
        return NULL;
    }
    return (void*)addr;
}

void page_free(void* addr) {
    uint64_t pfn = phy_to_pfn(virtual_to_physical((uint64_t)addr));
    page[pfn].used = AVAL;
}
