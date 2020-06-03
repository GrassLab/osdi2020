#ifndef _MM_H
#define _MM_H

#include "mmu.h"
#include "task.h"

#define PBASE 0x3F000000
#define PAGE_SHIFT	 	12
#define TABLE_SHIFT 		9
#define SECTION_SHIFT		(PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE   		(1 << PAGE_SHIFT)
#define SECTION_SIZE		(1 << SECTION_SHIFT)

#define LOW_MEMORY              (2 * SECTION_SIZE)
#define HIGH_MEMORY             PBASE

#define PAGING_MEMORY           (HIGH_MEMORY - LOW_MEMORY)
#define PAGING_PAGES            (PAGING_MEMORY/PAGE_SIZE)

#define PAGE_MAP_SIZE 0x10000

typedef enum {
    FREE,
    ALLOCATED,
} PAGE_STATUS;

struct page {
  unsigned long id;
  unsigned long virtual_addr;
  PAGE_STATUS status;
};

extern struct page page_map[PAGE_MAP_SIZE];

unsigned long get_free_page();
void free_page(unsigned long p);

void page_init();
struct page* page_alloc();
void page_free(struct page* p);
void page_mapping(struct task* task, struct page* user_page);
void move_ttbr(unsigned long* pgd);

#endif
