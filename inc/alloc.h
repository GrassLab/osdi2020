#include <stdint.h>
#include <stdbool.h>
#define MAX_ORDER 8

struct chunk_info_t{
    int size;
};

struct zone_node_t {
    struct zone_node_t* next;
    struct zone_node_t* prev;
    struct buddy_node_t* buddy_node;
    uint64_t order;
};

struct buddy_node_t {
    struct buddy_node_t* left;
    struct buddy_node_t* right;
    struct buddy_node_t* parent;
    struct zone_node_t zone_node;
    uint64_t addr;
    uint64_t size;
    bool used;
};

extern struct buddy_node_t buddy_node_pool[1024];
extern int buddy_node_pool_now;
extern struct buddy_node_t* buddy_system;
extern struct zone_node_t* zone[10];

struct buddy_node_t* new_buddy_node(uint64_t addr, uint64_t size);
void buddy_init();

void* kmalloc(uint64_t size);
void zone_push(struct zone_node_t* zone_node);
struct zone_node_t* zone_pop(uint64_t order);
struct buddy_node_t* get_buddy(uint64_t order);
