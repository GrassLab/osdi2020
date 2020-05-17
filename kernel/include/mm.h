#ifndef MM_H_
#define MM_H_

#include "types.h"

#define PHYSICAL_MEM_SIZE (1 << 30)
#define PAGE_SIZE (1 << 12)
#define PAGE_NUM (PHYSICAL_MEM_SIZE / PAGE_SIZE)

#define KVIRT_TO_PFN(addr) (uint64_t)((addr >> 12) & ((1ULL << 36) - 1))

struct page {
  bool inuse;
};

void page_init(void);
uint64_t page_alloc(void);
void page_free(uint64_t page_addr);

#endif // MM_H_