#ifndef MM_H_
#define MM_H_

#include "types.h"

#define PHYSICAL_MEM_SIZE (1 << 30)
#define PAGE_SIZE (1 << 12)
#define PAGE_NUM (PHYSICAL_MEM_SIZE / PAGE_SIZE)

struct page {
  bool inuse;
};

struct page pages[4096];

#endif // MM_H_