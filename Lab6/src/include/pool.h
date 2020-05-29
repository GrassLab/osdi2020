#ifndef _POOL_H
#define _POOL_H

#include "scheduler.h"

#define DEFAULT_ALLOCATOR_NUM 5
#define MIN_DEFAULT_ALLOCATOR_SIZE 0x100

#define MAX_ALLOCATOR_NUM 64 // this mean our os can only handle 64 allocator
#define MAX_POOL_PAGE   16// each allocator can use no more than 16 page

typedef struct free_list{
	struct free_list *next;
} free_list;

#define POOL_USE 0
#define POOL_NOT_USE 1
struct page_addr{
	int used;
	unsigned long phy_addr;
	unsigned long vir_addr;
};

typedef struct {
	free_list *free;	
	unsigned long element_size;
	unsigned int chunk_per_page;
	unsigned int used;
	int page; 
	//Note: we record virtual address
	struct page_addr pages_addr[MAX_POOL_PAGE]; 
} pool;

char allocator_used_map[MAX_ALLOCATOR_NUM];
pool obj_allocator[MAX_ALLOCATOR_NUM];

// since I am too weak so......
// just simply let every task has their own allocator
pool default_allocator[NR_TASKS][DEFAULT_ALLOCATOR_NUM]; 

void init_default_allocator(int pid);
void allocator_init();
int allocator_register(unsigned long size);
unsigned long allocator_kernel_alloc(int allocator_num);
unsigned long allocator_user_alloc(int allocator_num);
void allocator_free(int allocator_num,unsigned long ptr);
void allocator_kernel_unregister(int allocator_num);
void allocator_user_unregister(int allocator_num);

void poolFreeAll(pool *p);
void pool_init(pool *p,unsigned long element_size);
unsigned long pool_alloc_kernel(pool *p);
unsigned long pool_alloc_user(pool *p);
void pool_free(pool *p,unsigned long ptr);
void free_kernel_memory_pool(pool *p);
void free_user_memory_pool(pool *p);
#endif

