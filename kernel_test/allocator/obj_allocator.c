#include "buddy.h"
#include "obj_allocator.h"
#include "../kernel/uart.h"

/*
 ** Fixed-size object allocator
 */
void allocator_view() 
{
    for (int i=0; i<TOKEN_MAX_NUM; i++) {
        printf("token: %d\n", i);
        struct token_struct* tokenStruct = &AllocatorManager.token_pool[i];
        if (tokenStruct->size > 0) {
            printf("\t obj_size: 0x%lx\n", tokenStruct->size);
            struct mem_pool_struct* mem = &tokenStruct->mem_pool[0];
            printf("\t mem_pool[0].allocate_bitmap: 0x%lx\n", mem->allocate_bitmap);
            printf("\t mem_pool[0].page_addr: 0x%lx\n", mem->page_addr);
            printf("\t addr: [");
            for (int j=0; j<0x1000/tokenStruct->size; j++) {
                if (((mem->allocate_bitmap >> j) & 1) == 1) {
                    printf(" 0x%lx", mem->page_addr + j * tokenStruct->size);
                }
            }
            printf("]\n");
        } else {
            printf("\t None\n");
        }
    }
}

void init_allocator()
{
    AllocatorManager.token_num = 0;
    for (int i=0; i<TOKEN_MAX_NUM; i++) {
        AllocatorManager.token_pool[i].size = 0;
        AllocatorManager.token_pool[i].page_count = 0;
        AllocatorManager.token_pool[i].token = i;
    }
}

void init_token_struct(struct token_struct* tokenStruct)
{
    // printf("init_token_struct\n");
    int idx = tokenStruct->page_count;
    tokenStruct->mem_pool[idx].allocate_bitmap = 0;
    tokenStruct->mem_pool[idx].page_addr = malloc(0x1000); // get_free_page
    tokenStruct->page_count++;
}

unsigned long registration(int obj_size)
{
    int idx = AllocatorManager.token_num; 
    struct token_struct* tokenStruct = &AllocatorManager.token_pool[idx];
    tokenStruct->size = obj_size;
    AllocatorManager.token_num += 1;

    return tokenStruct->token;
}
/*
 ** By default, we assume alloc range is from 0x10 to 0x100
 ** So, there is no need to allocate more than one page,
 ** Eg. 0x1000 bytes / 0x100 size = 0x10 mem_chunks 
 */
unsigned long fixed_alloc(unsigned long token)
{
    // Check the token and find the memory pool.
    unsigned long addr, page_addr;
    struct mem_pool_struct* mem;
    struct token_struct* tokenStruct = &AllocatorManager.token_pool[token];
    
    // If the memory pool is empty, allocate page frames.
    //     Initialize the memory pool for bookkeeping.
    if (tokenStruct->page_count == 0) {
        init_token_struct(&AllocatorManager.token_pool[token]);
    }
    mem = &tokenStruct->mem_pool[tokenStruct->page_count-1];
    // Find empty slot in the memory pool and return it to user
    for (int i=0; i<0x1000/tokenStruct->size; i++) {
        // printf("[%d] allocate_bitmap: 0x%lx\n", i, mem->allocate_bitmap);
        
        if (((mem->allocate_bitmap >> i) & 1) == 0) {
            mem->allocate_bitmap ^= (1 << i);
            // printf("allocate_bitmap: 0x%lx\n", mem->allocate_bitmap);
            return addr = mem->page_addr + i * tokenStruct->size;
        }
    }
    return 0;
}

// The user frees the object by 
//     passing the address of the object as an argument to the allocator. 
// The allocator should look up its internal data structure 
//     and put the object to the right pool.
int fixed_free(unsigned long addr)
{

    // find the token
    for (int i=0; i<AllocatorManager.token_num; i++) {
        struct token_struct* tokenStruct = &AllocatorManager.token_pool[i];
        int page_count = tokenStruct->page_count;
        for (int j=0; j<page_count; j++) {
            struct mem_pool_struct* mem = &tokenStruct->mem_pool[j];
            unsigned long offset = addr - mem->page_addr;
            if ((offset >= 0) & (offset < 0x1000)) { 
                printf("free\n");
                int bit = offset/tokenStruct->size; // must dividable because of alignment
                if (((mem->allocate_bitmap >> bit) & 1) == 1) {
                    mem->allocate_bitmap ^= (1 << bit);
                    printf("after free allocate_bitmap: 0x%lx\n", mem->allocate_bitmap);
                    return 0;
                } else {
                    printf("double free\n");
                    return 1; 
                }

            }
        }
    }
    // invalid free by no such addr
    return 1; // should not be here
}

/*
 ** Varied-size object allocator (proxy of page allocator and fixed-sized allocator)
 ** policy:
 **     >  0x1000 (one page) --> buddy system
 **     <= 0x1000            --> object allocator
 */


// If the allocated size is very big, 
//      it calls page allocator directly to allocate contiguous page frames. 
// Otherwise, 
//      it rounds up the allocated size to one of the registered allocators and uses the token to the fixed-sized allocator.

unsigned long malloc(int size)
{
    if (size >= 0x1000) {
        int buddy_size = size >> 12; // per page
        if ((size & 0xfff) > 0) {
            buddy_size++;
        }
        printf("buddy size: %d\n", buddy_size);
        return buddy_alloc(buddy_size);
    } else {
        // find token by size
        for (int i=0; i<AllocatorManager.token_num; i++) {
            if (AllocatorManager.token_pool[i].size == size) {
                return fixed_alloc(i);
            }
        }
        // not found <-> first use
        unsigned long token = registration(size);
        return fixed_alloc(token);
    }
    return 0;
}

int free(unsigned long addr)
{
    // check if this addr is buddy
    if (buddy_free(addr) == 0) {
        return 0;
    }
    printf("varied_free error\n");
    if (fixed_free(addr) == 0) {
        return 0;
    }
    return 1; // free error
}