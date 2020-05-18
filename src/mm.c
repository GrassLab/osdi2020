#include "mm.h"
#include "typedef.h"
#include "peripherals/mmio.h"

struct page_t page[PAGE_FRAMES_NUM];

void mm_init() {
    extern unsigned char __kernel_end; // defined in linker
    uint64_t kernel_end = (uint64_t)&__kernel_end;
    int kernel_end_page = kernel_end / PAGE_SIZE;
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
}