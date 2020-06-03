#include "buddy.h"

#define INIT_BITMAP (1 << BUDDY_MAX_ORDER)
#define BITMAP(order) (1 << order)
// #define CHECK_BITMAP(order) (((BuddyManager.allocate_bitmap >> order) & 1) == 1)


void buddy_view() 
{
    for (int i=0; i<BUDDY_MAX_ORDER; i++) {
        printf("order: %d,\t", i+1);
        if (BuddyManager.list[i].number > 0) {
            printf("num: %d, ", BuddyManager.list[i].number);
            printf("addr: [");
            for (int j=0; j<BuddyManager.list[i].number; j++) {
                printf("0x%lx, ", BuddyManager.list[i].chunk[j].addr);
            }
            printf("]\n");
        } else {
            printf("addr: [none]\n");
        }
    }
    printf("BuddyManager.allocate_bitmap: 0x%lx\n", BuddyManager.allocate_bitmap);
}

void buddy_init()
{
    unsigned long init_chunk_addr;
    init_chunk_addr = malloc(INIT_BITMAP); // the malloc should be replace to get_free_page
    // for (int i=0; i<BUDDY_MAX_ORDER; i++) {
    //     BuddyManager.list[i].number = 0;
    // }
    BuddyManager.list[BUDDY_MAX_ORDER-1].chunk[0].addr = init_chunk_addr;
    BuddyManager.list[BUDDY_MAX_ORDER-1].chunk[0].state = NOT_IN_USE;
    BuddyManager.list[BUDDY_MAX_ORDER-1].number = 1;
    BuddyManager.allocate_bitmap = 0x0;
}


unsigned long buddy_alloc(int size)
{
    int current_order = BUDDY_MAX_ORDER;
    int is_align = 0; // check if there is a non-2-power request
    int return_order = 0;
    
    for (int i=BUDDY_MAX_ORDER; i>=0; i--) {
        if (((size >> i) & 1) == 1) {
            if (is_align == 0){ // there exist one bit
                return_order = i;
                is_align = 1;
            } else { // there exist two bits
                return_order += 1;
                break;
            }
        }
    }

    // printf("return_order: %d\n", return_order);
    
    return get_buddy_chunk(return_order);
}

int buddy_free(unsigned long addr)
{
    // traverse the same order, and find out the the same addr
    struct buddy_chunk* chunk;
    int order, chunk_idx;
    for (order=BUDDY_MAX_ORDER; order>0; order--) {
        if (((BuddyManager.allocate_bitmap >> order) & 1) == 1) {
            // printf("order:  %d\n", order);
            for (chunk_idx=0; chunk_idx<BuddyManager.list[order-1].number; chunk_idx++) {
                chunk = &BuddyManager.list[order-1].chunk[chunk_idx];
                // printf("chunk addr: 0x%lx\n", chunk->addr);
                if ((chunk->addr == addr) & (chunk->state == IN_USE)) {
                    chunk->state == NOT_IN_USE;
                    BuddyManager.allocate_bitmap ^= BITMAP(order);
                    // printf("found chunk\n");
                    merge_buddy_chunk(order, addr, chunk_idx);
                    return 0;
                }
            } 
        }
    }
    // if contigious exist -> merge

    return 1;   
}


/* 
 ** find a chunk fit the order
 */
unsigned long get_buddy_chunk(int order)
{
    struct buddy_chunk* chunk;
    int error;
    if (order > BUDDY_MAX_ORDER) {
        return 0;    
    }
    if (BuddyManager.list[order-1].number == 0) {
        if (order == BUDDY_MAX_ORDER) {
            return 0;
        }
        for (int i=order; i<BUDDY_MAX_ORDER; i++) {
            if (BuddyManager.list[i].number > 0) {
                error = split_buddy_chunk_to_order(i, order);
                if (error == 1) {
                    return 0;
                }
                break;
            }
        }
    }
    for (int i=BuddyManager.list[order-1].number-1; i>=0; i--) {
        chunk = &BuddyManager.list[order-1].chunk[i];
        if (chunk->state == NOT_IN_USE) {
            chunk->state = IN_USE;
            BuddyManager.allocate_bitmap ^= BITMAP(order);
            return chunk->addr;
        }

    }
    return 0;
}

void remove_buddy_chunk(int order, int idx)
{
    BuddyManager.list[order].chunk[idx].addr = 0;
    BuddyManager.list[order].number -= 1;
}

void put_buddy_chunk(unsigned long addr, int order)
{
    int next_idx = BuddyManager.list[order].number;
    BuddyManager.list[order].number += 1;
    BuddyManager.list[order].chunk[next_idx].addr = addr;
    BuddyManager.list[order].chunk[next_idx].state = NOT_IN_USE;
    // printf("addr: 0x%lx\n", addr);
}

// void unlink_from_tail(struct buddy_chunk* chunk, int order)
// {
//     struct buddy_chunk* tmp;
//     tmp = chunk->prev;
//     tmp->next = NULL;
//     tmp->prev = NULL;
//     BuddyManager.list[order].tail = tmp;
// }

int split_buddy_chunk_to_order(int current_order, int target_order)
{
    for (int i=current_order; i>=target_order; i--) { 
        int error = split_buddy_chunk(i);
        if (error == 1) {
            printf("Error: split fail.\n");
            return 1;
        }
        // buddy_view();
    }
    return 0;
}

int split_buddy_chunk(int order)
{
    struct buddy_chunk* chunk;
    if (BuddyManager.list[order].number == 0) {
        return 1;
    }
    for (int i=BuddyManager.list[order].number-1; i>=0; i--) {
        chunk = &BuddyManager.list[order].chunk[i];
        if ((chunk->state == NOT_IN_USE) & (chunk->addr != 0)) {
            unsigned long addr1, addr2;
            addr1 = chunk->addr;
            addr2 = chunk->addr + (1 << (order));
            // printf("split order: %d\n", order);
            // printf("chunk->state:  %d\n", chunk->state);
            put_buddy_chunk(addr1, order-1);
            put_buddy_chunk(addr2, order-1);
            remove_buddy_chunk(order, i);
            // chunk->addr = 0;
            // BuddyManager.list[order].number -= 1;
            return 0;
        }
    }
    return 1;
}

void merge_buddy_chunk(int order, unsigned long addr, int idx)
{
    for (int i=order; i<BUDDY_MAX_ORDER; i++) {
        // printf("order: %d\n", i);
        unsigned long prev_addr = addr - BITMAP(i);
        unsigned long next_addr = addr + BITMAP(i);
        for (int j=BuddyManager.list[i-1].number-1; j>=0; j--) {
            if (BuddyManager.list[i-1].chunk[j].addr == prev_addr) {
                printf("merge prev, chunk addr: 0x%lx\n", prev_addr);
                remove_buddy_chunk(i-1, idx);
                remove_buddy_chunk(i-1, j);
                put_buddy_chunk(prev_addr, i);
                addr = prev_addr;
                // buddy_view();
                break;
            }
            if (BuddyManager.list[i-1].chunk[j].addr == next_addr) {
                printf("merge next, chunk addr: 0x%lx\n", prev_addr);
                remove_buddy_chunk(i-1, idx);
                remove_buddy_chunk(i-1, j);
                put_buddy_chunk(addr, i);
                // buddy_view();
                break;
            }
        }
    }
}
