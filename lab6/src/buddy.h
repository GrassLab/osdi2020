#pragma once


#define MAX_ORDER 11
#define MAX_ALLOCATOR_NUMBER 16
#define MIN_ALLOCATOR_SIZE 8
#define MIN_KMALLOC_ORDER 4
#define MAX_KMALLOC_ORDER 12

struct buddy_head{
    int len;
    struct page* page;
};

struct buddy_head buddy_pool[MAX_ORDER];

struct page{
    long long block;
    int page_frame_number;
    unsigned long physical_addr;
    unsigned int size;
    struct page* next;

    struct obj_alloc *obj_alloc;
    unsigned int obj_size;
    unsigned int unused_obj;
    void **obj_freelist;
    void **obj_usedlist;
};

struct obj_alloc{
    struct page* curr_page;
    struct page* used_page;
    unsigned int obj_size;
    unsigned int used;    
};

struct obj_alloc allocator_pool[MAX_ALLOCATOR_NUMBER];

int cal_order(int size);
void buddy_init(int num_pages);
void buddy_show();
struct page *buddy_alloc(int size);
int buddy_alloc_ret_pfn(int size);
int pfn2phy(int page_frame_number);
int phy2pfn(int physical_addr);
void check_merge();
void buddy_free(int page_frame_number, int page_frame_size);
void _buddy_free(int page_frame_number, void *block);

void __init_obj_alloc(struct obj_alloc *alloc, unsigned int size);
int register_obj_allocator(unsigned int obj_size);
void *obj_allocate(int token);
void __init_obj_page(struct page* page, unsigned size);
void _obj_free(void *obj, struct page *page);
void obj_free(void *obj, int token);

void *kmalloc(unsigned int size);
void kfree(void *block);
struct page *find_page(int pfn);