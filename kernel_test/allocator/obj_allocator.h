#define TOKEN_MAX_NUM 0x10
#define MEM_POOL_NUM 0x10

struct mem_pool_struct {
    unsigned long allocate_bitmap; // only record 16-bit
    // char allocate_bitmap[0x100];
    unsigned long page_addr;
};

struct token_struct {
    int size;
    int page_count;
    unsigned long token;
    struct mem_pool_struct mem_pool[MEM_POOL_NUM]; //pointer to page frame
};

struct allocator_manager {
    int token_num;
    struct token_struct token_pool[TOKEN_MAX_NUM];
};

struct allocator_manager AllocatorManager;

void init_allocator();
void init_token_struct(struct token_struct* t);
unsigned long registration(int obj_size);
unsigned long fixed_alloc(unsigned long token);
int fixed_free(unsigned long  addr);
void allocator_view();


unsigned long malloc(int size);
int free(unsigned long addr);