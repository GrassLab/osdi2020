#ifndef _POOL_H
#define _POOL_H

#include "mm.h"

#define MAX_POOL_PAGE 8

typedef struct free_list{
	struct free_list *next;
} free_list;

typedef struct {
	free_list *free;	
	struct list_head list;
	unsigned long element_size;
	unsigned int chunk_per_page;
	unsigned int used;
	
	int page;
	struct page_struct *pages[MAX_POOL_PAGE]; //now the pool can use only no more than 8 page
} pool;

void poolFreeAll(pool *p);
void pool_init(pool *p,unsigned long element_size);
unsigned long pool_alloc(pool *p);
void pool_free(pool *p,unsigned long ptr);
void free_memory_pool(pool *p);
#endif

