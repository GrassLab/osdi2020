#ifndef POOL_H
#define POOL_H
#include <stdbool.h>
#include "alloc.h"
#define MAX_PROCESSES 5
#define MAX_POOL 10
#define POOL_SIZE 1
#define MAX_BOOKS 1024 * 4 * 2

struct pool_t {
    int size;
    struct buddy_node_t* buddy_node;
    bool used;
    bool bookkeeping[MAX_BOOKS];
};

void init_pool(struct pool_t* pool, int size);
void register_pool(int pid, int size);
void* alloc_object(struct pool_t* pool);
void* kmalloc(int size);
void kfree(void* addr);

extern struct pool_t pool_allocators[MAX_PROCESSES][MAX_POOL];
extern int pool_now;
#endif
