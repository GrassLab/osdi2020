#ifndef __ALLOCATOR_H__
#define __ALLOCATOR_H__

#define FIX_ALLOCATOR_PFN_SIZE 	(8)
#define PAGE_SIZE 				(1<<12)
#define FIX_ALLOCATOR_MEM_POOL_FULL (0xFFFFFFFFFFFFFFFF)

typedef struct{
	unsigned long long mem_pool[FIX_ALLOCATOR_PFN_SIZE];
	int PFN[FIX_ALLOCATOR_PFN_SIZE];
	int PFN_num;
	int obj_size;
	unsigned long long mem_pool_init;
} fix_allocator;

typedef struct{
	fix_allocator *fix_allocator_512;
	fix_allocator *fix_allocator_2048;
	fix_allocator *fix_allocator_4096;
} dynamic_allocator;

fix_allocator* fix_allocator_init(int size);
void* fix_alloc(fix_allocator *self);
int fix_free(fix_allocator *self, void* addr);

dynamic_allocator* dynamic_allocator_init(void);
void* dynamic_alloc(dynamic_allocator *self, int req_size);
void dynamic_free(dynamic_allocator *self, void* addr);
#endif