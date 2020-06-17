#define PAGE_SIZE 4096

#define PAGE_POW 9
#define NUM_PAGE (1<<PAGE_POW)
#define LINK_LIST_SIZE (16)
#define ALLOCATED 1
#define NO_ALLOCATED 0
#define USAGE 2

typedef struct link_list{
	unsigned long long pgnum;
    struct link_list* next;
    unsigned long long ppow;
} link_list;

typedef struct obj_struct{
	unsigned long long obj_size;
    unsigned long long bitmap_size; //number of bit
    struct obj_struct* next;
} obj_struct;

#define REAL_OBJ_PAGE_SIZE (PAGE_SIZE-sizeof(link_list)-sizeof(obj_struct))
#define REAL_PAGE_SIZE (PAGE_SIZE-sizeof(link_list))
void link_init();
char* mmap(unsigned long long size);
int merge_check(link_list* chunk_check);
int free(void *fre);
void print_link_header_state();
int registe_object(unsigned long long size);
void *object_allocate(int pgnum);
void print_obj_bitmap(int pgnum);
void show_obj_link();
_Bool object_free(void* obj);
void object_logout(int pgnum);
void* varied_allocate(unsigned long long size);
int varied_free(void *fre);