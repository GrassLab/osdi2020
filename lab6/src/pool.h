#include "mm.h"
#define NO_USED 0
#define USED 1
#define NUM_ALLOCATOR 64

struct st_pool{
    unsigned long start_addr;
    unsigned long book_info;
    unsigned long size;
    unsigned long num_chunk;
};

struct st_allocator{
    int flag;
    struct st_pool pool;    
}; 

struct st_allocator allocators[NUM_ALLOCATOR];

void init_allocator();
unsigned long register_allocator(unsigned long size);
unsigned long allocate(unsigned long token);
void free(unsigned long token, unsigned long ptr);

