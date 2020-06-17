#ifndef PAGE_H
#define PAGE_H

#include "mm.h"
#include "task.h"
#include "alloc.h"
#include "pool.h"
#include <stdint.h>
#define PAGE_MAX 10000
typedef enum {
    FREE,
    ALLOCATED,
} PAGE_STATUS;
struct page_t {
    int id;
    PAGE_STATUS status;
    uint64_t *content;
    struct pool_t* pool;
    struct buddy_node_t* buddy_node;
};

extern struct page_t pages[PAGE_MAX];
extern int page_now;

struct page_t* page_alloc();
void page_free(struct page_t* page);
void page_mapping(struct task_t* task, struct page_t* page);
void stack_mapping(struct task_t* task, struct page_t* stack_page);
void move_ttbr(uint64_t *pgd);
#endif
