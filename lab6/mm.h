#ifndef _MM_H
#define _MM_H

#include "base.h"

#define PAGE_SHIFT 12
#define TABLE_SHIFT 9
#define SECTION_SHIFT (PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE (1 << PAGE_SHIFT)
#define SECTION_SIZE (1 << SECTION_SHIFT)

#define LOW_MEMORY (2 * SECTION_SIZE)
#define HIGH_MEMORY PBASE

// 1GB
#define PAGING_MEMORY ((1 << 16) * PAGE_SIZE)
#define PAGING_PAGES (PAGING_MEMORY / PAGE_SIZE)

// for variable-size allocator
#define VARIABLE_LOW_MEMORY (LOW_MEMORY + PAGING_MEMORY)

typedef struct free_area {
    struct free_area* next;
    unsigned long* map;
} free_area_t;

void* malloc(unsigned int size);
void free_memory(unsigned long addr);
void init_memory();
void print_memory_pool();
void variable_free(unsigned int addr);
void* variable_malloc(unsigned int size);
void print_variable_memory_pool();

#endif