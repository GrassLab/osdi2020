#include "alloc.h"
#include "io.h"
#include "page.h"
#include "pool.h"
#include "task.h"
struct pool_t pool_allocators[MAX_PROCESSES][MAX_POOL];
int pool_now;

void init_pool(struct pool_t* pool, int size) {
    pool->size = size;
    pool->used = true;
    /* print_s("pool is empty, allocte from buddy\n"); */
    pool->buddy_node = alloc_buddy(POOL_SIZE);
    for (int i = 0; i < 1 << POOL_SIZE; i++) {
        pages[((pool->buddy_node->addr ^ 0xffff000000000000) >> 12) + i].pool =
            pool;
    }
    for (int i = 0; i < MAX_BOOKS; i++) {
        pool->bookkeeping[i] = false;
    }
}

void register_pool(int pid, int size) {
    /* print_s("registering pool...\n"); */
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
    /* print_s("kmalloc size: "); */
    /* print_i(size); */
    /* print_s("B"); */
    struct task_t* task = get_current();
    void* addr = 0;
    if (size >= 1024 * 4) {
        /* print_s(" with buddy\n"); */
        int target_size = 1024 * 4;
        int order = 0;
        while (size > target_size) {
            target_size <<= 1;
            order++;
        }
        struct buddy_node_t* buddy_node = alloc_buddy(order);
        addr = (void*)buddy_node->addr;
        pages[(buddy_node->addr ^ 0xffff000000000000) >> 12].buddy_node =
            buddy_node;
    } else {
        /* print_s(" with pool\n"); */
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
    }
    return addr;
}

void kfree(void* addr) {
    int64_t addr_machine = (int64_t)addr ^ 0xffff000000000000;
    /* print_s("kfree addr: "); */
    /* print_h((uint64_t)addr); */
    struct page_t* page = &pages[addr_machine >> 12];
    if (page->pool) {
        /* print_s(" in pool\n"); */
        struct pool_t* pool = page->pool;
        pool->bookkeeping[(int64_t)addr & 0xfff / pool->size] = false;
    } else {
        /* print_s(" in buddy\n"); */
        free_buddy(page->buddy_node);
        page->buddy_node = 0;
    }
}
