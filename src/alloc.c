#include "alloc.h"
#include "page.h"
#include "task.h"

struct buddy_node_t buddy_node_pool[1024];
struct buddy_node_t* buddy_system;
struct zone_node_t* zone[10];
int buddy_node_pool_now = 0;

struct buddy_node_t* new_buddy_node(uint64_t addr, uint64_t size) {
    struct buddy_node_t* node = &buddy_node_pool[buddy_node_pool_now++];
    node->used = false;
    node->addr = addr + 0xffff000000000000;
    if (size == 1) {
        node->left = 0;
        node->right = 0;
        return node;
    }
    node->left = new_buddy_node(addr, size / 2);
    node->right = new_buddy_node(addr + size / 2 * 0x1000, size / 2);
    node->left->parent = node;
    node->right->parent = node;
    node->size = size;
    node->zone_node.order = __builtin_ctz(size);  // !!!!!!1
    node->zone_node.buddy_node = node;
    return node;
}

void buddy_init() {
    struct page_t* page = page_alloc();
    for (int i = 0; i < 255; i++) {
        page_alloc();
    }
    buddy_system = new_buddy_node((uint64_t)page->content, 1 << MAX_ORDER);
    zone[MAX_ORDER] = &buddy_system->zone_node;
    asm volatile("b:");
}

struct buddy_node_t* alloc_buddy(uint64_t size) {
    uint64_t target_size = 1;
    int order = 0;
    while (size > target_size) {
        target_size <<= 1;
        order++;
    }
    struct buddy_node_t* buddy_node = get_buddy_by_order(order);
    return buddy_node;
}

void free_buddy(struct buddy_node_t* buddy_node) {
    buddy_node->used = false;
    if (buddy_node->parent->left == buddy_node &&
        buddy_node->right->used == false) {
        buddy_node->parent->used = false;
        zone_delete(&buddy_node->parent->left->zone_node);
        free_buddy(buddy_node->parent->zone_node.buddy_node);
    } else if (buddy_node->parent->right == buddy_node &&
               buddy_node->left->used == false) {
        buddy_node->parent->used = false;
        zone_delete(&buddy_node->parent->left->zone_node);
        zone_push(&buddy_node->parent->zone_node);
        free_buddy(buddy_node->parent->zone_node.buddy_node);
    } else {
        zone_push(&buddy_node->zone_node);
    }
}

void zone_delete(struct zone_node_t* zone_node) {
    if (!zone_node->prev && !zone_node->next) {
        zone[zone_node->order] = 0;
        return;
    }
    if (zone_node->prev) {
        zone_node->prev->next = zone_node->next;
    }
    if (zone_node->next) {
        zone_node->next->prev = zone_node->prev;
    }
}

void zone_push(struct zone_node_t* zone_node) {
    zone_node->next = zone[zone_node->order];
    zone[zone_node->order]->prev = zone_node;
    zone[zone_node->order] = zone_node;
}

struct zone_node_t* zone_pop(uint64_t order) {
    struct zone_node_t* zone_node = zone[order];
    if (zone_node->next) {
        zone_node->next->prev = 0;
    }
    zone[order] = zone_node->next;
    return zone_node;
}

struct buddy_node_t* get_buddy_by_order(uint64_t order) {
    if (order > MAX_ORDER) {
        return 0;
    }
    if (!zone[order]) {
        struct buddy_node_t* buddy = get_buddy_by_order(order + 1);
        buddy->used = true;
        buddy->left->used = true;
        zone_push(&buddy->right->zone_node);
        return buddy->left;
    } else {
        struct zone_node_t* zone_node = zone_pop(order);
        return zone_node->buddy_node;
    }
}
