#define BUDDY_LIST_NUM  16
#define BUDDY_MAX_ORDER 12
#define NOT_IN_USE  0
#define IN_USE      1

struct buddy_chunk {
    unsigned long addr; // equivlent to PFN
    int state;
};

struct buddy_list {
    int number;
    struct buddy_chunk chunk[BUDDY_LIST_NUM];
};

struct buddy_manager {
    int allocate_bitmap;
    struct buddy_list list[BUDDY_MAX_ORDER];
};

struct buddy_manager BuddyManager;

void buddy_view();
void buddy_init();
unsigned long buddy_alloc(int size);
int buddy_free(unsigned long addr);
unsigned long get_buddy_chunk(int order);
void put_buddy_chunk(unsigned long addr, int order);
void remove_buddy_chunk(int order, int idx);
int split_buddy_chunk_to_order(int current_order, int target_order);
int split_buddy_chunk(int order);
void merge_buddy_chunk(int order, unsigned long addr, int idx);