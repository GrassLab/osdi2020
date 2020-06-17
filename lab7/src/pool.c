#include "pool.h"
void init_allocator(){
    for(int i = 0; i < NUM_ALLOCATOR; i++){
        allocators[i].flag = NO_USED;
    }
}

unsigned long register_allocator(unsigned long size){
    for(int i = 0; i < NUM_ALLOCATOR; i++){
        if (allocators[i].flag == NO_USED){
            allocators[i].flag = USED;
            allocators[i].pool.size = size;
            
            int order = 4;
            int num_chunk = (1<<order) * PAGE_SIZE / (size + 1);
            allocators[i].pool.num_chunk = num_chunk;

            unsigned long addr = (unsigned long) get_free_page(order);
            allocators[i].pool.book_info = addr;
            char *book_info_addr = (char *)addr;

            for (int j = 0; j < num_chunk; j++){
                *(book_info_addr + j) = NO_USED;
            }
            allocators[i].pool.start_addr = (unsigned long)(book_info_addr + 1 * num_chunk);            
            return i;
        }
    }
    return -1;
}

unsigned long chunk_allocate(unsigned long token){
    //check the token 
    if(allocators[token].flag != USED){
        uart_puts("invalid token, please register_allocator !");
    }
    //find empty slot in the memory pool 
    char *book_info_addr = (char *)allocators[token].pool.book_info; 
    int num_chunk = allocators[token].pool.num_chunk;
    int size = allocators[token].pool.size;
    char *start_addr = (char *)allocators[token].pool.start_addr;
    for(int i = 0; i < num_chunk; i++) {
        if( *(book_info_addr + i) == NO_USED){
            *(book_info_addr + i) = USED;
            return (unsigned long)(start_addr + i * size);
        }
    }
    return -1;
}

void chunk_free(unsigned long token, unsigned long ptr){
    char *book_info_addr = (char *)allocators[token].pool.book_info;
    unsigned long start_addr = allocators[token].pool.start_addr;
    int index = (ptr - start_addr) / allocators[token].pool.size;
    *(book_info_addr + index) = NO_USED;
}


