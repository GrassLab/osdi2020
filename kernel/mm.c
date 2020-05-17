#include "kernel/mm.h"

extern char __kernel_start[];
extern char __kernel_end[];
struct page pages[PAGE_NUM];

void page_init(void) {
  for (uint64_t pfn = KVIRT_TO_PFN((uint64_t)__kernel_start); pfn <= KVIRT_TO_PFN((uint64_t)__kernel_end); ++pfn) {
    pages[pfn].inuse = true;
  }
}

uint64_t page_alloc(void) {
  for (uint64_t i = 0; i < PAGE_NUM; ++i) {
    if (!pages[i].inuse) {
      pages[i].inuse = true;
      return i << 12;
    }
  }
  return (uint64_t)-1;
}

void page_free(uint64_t page_addr) {
  pages[KVIRT_TO_PFN(page_addr)].inuse = false;
}