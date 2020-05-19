// page state
#define NOT_USED    0 
#define IN_USED     1
#define PRESERVE    2

#define PAGING_PAGES 0x3efff

struct page {
    int state; // 0 for NOT_USED; 1 for USED_NOW; 2 for PRESERVE;
};

struct page_manager {
    int remain_page;
    struct page pages[PAGING_PAGES];
};


void init_page();
unsigned long virtual_to_physical(unsigned long va);
unsigned long allocate_kernel_page();
unsigned long get_free_page();
void free_page(unsigned long p);
unsigned long allocate_user_page(struct task *task, unsigned long va);
void map_page(struct task *task, unsigned long va, unsigned long page, unsigned long page_attr);
unsigned long map_table(unsigned long *table, unsigned long shift, unsigned long va, struct task *task);
void map_entry(unsigned long *pte, unsigned long va, unsigned long pa, unsigned long page_attr);