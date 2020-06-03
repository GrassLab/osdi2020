#ifndef _SLAB_H
#define _SLAB_H

#include "buddy.h"
#define NUM_ALLOCATOR 20
typedef struct chunk_struct{
    unsigned long addr;
    int size;
    struct chunk_struct* next;
}chunk;

typedef struct allocator_struct{
    int size;
    int len;
    chunk* chunk_head;
    
}allocator;

int allocator_used[NUM_ALLOCATOR];
allocator allocator_pool[NUM_ALLOCATOR];

void init_all_allocator();

int init_allocator(int size);
void show_allocator(int allocator_id);
void free_alloc(int allocator_id, int addr);
int alloc(int allocator_id);

void init_var_allocator();
int var_alloc(int size);
void var_free(int address, int size);
#endif