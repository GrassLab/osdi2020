#ifndef _MM_H
#define _MM_H
#include "peripherals/base.h"

#define PAGE_SHIFT              12
#define TABLE_SHIFT             9
#define SECTION_SHIFT           (PAGE_SHIFT + TABLE_SHIFT)
#define PAGE_SIZE               (1 << PAGE_SHIFT)
#define SECTION_SIZE            (1 << SECTION_SHIFT)

#define LOW_MEMORY              (2 * SECTION_SIZE) // 4MB
#define HIGH_MEMORY PBASE
#define NUM_PAGES               64     // deprecated
#define KERNEL_PAGE_NUM         4096

#define PAGE_MASK               (~(PAGE_SIZE-1))

#define KERNEL_PAGING_START     (LOW_MEMORY + NUM_PAGES * PAGE_SIZE)
#define KERNEL_PAGING_END       (KERNEL_PAGING_START + KERNEL_PAGE_NUM * PAGE_SIZE)

#define KERNEL_PAGE_START       (KERNEL_PAGING_END)
#define KERNEL_PAGE_END         (KERNEL_PAGE_START + KERNEL_PAGE_NUM * sizeof(struct page))

#define MAX_BUDDY_PAGE_NUM      10
#define PAGE_NUM_FOR_MAX_BUDDY  ((1 << (MAX_BUDDY_PAGE_NUM - 1)) - 1)

#define PAGING_MEMORY           (HIGH_MEMORY - LOW_MEMORY)

#define NUM_ALLOCATORS          64
#define ALLOCATOR_ORDER         6
#define NUM_DYNAMIC_ALLOC       64

#ifndef __ASSEMBLER__
struct list_head {
    struct list_head *next, *prev;
};

#define PAGE_AVAILABLE      0x00
#define PAGE_DIRTY          0X01
#define PAGE_PROTECT        0X02
#define PAGE_BUDDY_BUSY     0x04
#define PAGE_IN_CACHE       0x08

struct page {
    unsigned long vaddr;
    unsigned long flags;
    int order;
    unsigned long counter;
    struct list_head list;
};

void memzero(unsigned long src, unsigned long n);
void memcpy(unsigned long dst, unsigned long src, unsigned long n);
unsigned long allocate_task_struct();
void free_task_struct(unsigned long ptr);
void show_buddy_system();
void init_page_map();
void* get_free_pages(unsigned long, int);
void put_free_pages(void*, int);

/* fixed sized allocator */
struct allocator {
    unsigned long base_addr;
    unsigned long addr;
    unsigned long chunk_size;
    unsigned long num_chunk;
};

extern char allocator_used_map[NUM_ALLOCATORS];
extern struct allocator allocator_pool[NUM_ALLOCATORS];

unsigned long allocator_register(unsigned long);
unsigned long allocator_alloc(unsigned long);
void allocator_free(unsigned long, unsigned long);
void allocator_unregister(unsigned long);

/* varied-sized alloc */
struct dalloc_info {
    char used;
    unsigned long addr;
    unsigned long size;
};

extern struct dalloc_info dynamic_alloc_pool[NUM_DYNAMIC_ALLOC];
unsigned long dynamic_alloc(unsigned long size);
void dynamic_free(unsigned long addr);

#endif

#endif /*_MM_H */
