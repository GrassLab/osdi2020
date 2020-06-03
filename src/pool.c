#include "alloc.h"
#include "pool.h"
#include "task.h"
struct pool_t pool_allocators[MAX_PROCESSES][MAX_POOL];
int pool_now;

void init_pool(struct pool_t* pool, int size) {
    pool->size = size;
    pool->used = true;
    pool->buddy_node = alloc_buddy(POOL_SIZE);
    for (int i = 0; i < MAX_BOOKS; i++) {
        pool->bookkeeping[i] = false;
    }
}

void register_pool(int pid, int size) {
    for (int i = 0; i < MAX_POOL; i++) {
        if (!pool_allocators[pid][i].used) {
            pool_allocators[pid][i].size = size;
        }
    }
}

void* alloc_object(struct pool_t* pool) {
    for (int i = 0; i < MAX_BOOKS; i++) {
        if (!pool->bookkeeping[i]) {
            asm volatile("h:");
            pool->bookkeeping[i] = true;
            return (void*)(pool->buddy_node->addr + pool->size * i);
        }
    }
    return 0;
}

void* kmalloc(int size) {
    struct task_t* task = get_current();
    void* addr = 0;
    for (int i = 0; i < MAX_POOL; i++) {
        if (pool_allocators[task->id][i].size == size) {
            addr = alloc_object(&pool_allocators[task->id][i]);
            break;
        }
        if (!pool_allocators[task->id][i].used) {
            init_pool(&pool_allocators[task->id][i], size);
            addr = alloc_object(&pool_allocators[task->id][i]);
            break;
        }
    }
    return addr;
}
