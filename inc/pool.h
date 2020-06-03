#ifndef POOL_H
#define POOL_H
#include <stdbool.h>
#include "alloc.h"
#define MAX_PROCESSES 5
#define MAX_POOL 10
#define POOL_SIZE 2
#define DEFAULT_POOL 3
#define MAX_BOOKS 1024 * 4 * 2

struct pool_t {
    int size;
    struct buddy_node_t* buddy_node;
    bool used;
    bool bookkeeping[MAX_BOOKS];
};

void* kmalloc(int size);

extern struct pool_t pool_allocators[MAX_PROCESSES][MAX_POOL];
extern int pool_now;
#endif
