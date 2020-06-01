#include "mm.h"

#include "printf.h"
#include "schedule.h"
#include "uart.h"


static char kstack_pool[NR_TASKS][THREAD_SIZE];
static char ustack_pool[NR_TASKS][THREAD_SIZE];

unsigned long get_kstack_base(unsigned long task_id){
    return (unsigned long)(kstack_pool[task_id]);
}

unsigned long get_ustack_base(unsigned long task_id){
    return (unsigned long)(ustack_pool[task_id]);
}

void __init_page_struct()
{
    for(int i=0; i<NR_PAGE; i++){
        bookkeep[i].pfn = i;
        bookkeep[i].used = 0;
        bookkeep[i].order = -1;
        bookkeep[i].phy_addr = LOW_MEMORY + i*PAGE_SIZE;
        INIT_LIST_HEAD(&(bookkeep[i].list));// feature envy?
    }
}

void __init_buddy()
{
    for(int i=0; i<MAX_ORDER+1; i++){
        INIT_LIST_HEAD(&free_list[i]);
    }

    for(int i=0; i<NR_PAGE; i+=MAX_ORDER_SIZE){
        list_add_tail(&bookkeep[i].list, &free_list[MAX_ORDER]);
    }
}

void mm_init()
{
    __init_page_struct();// must do before buddy or it will break the list
    __init_buddy();
}

struct page *__buddy_block_alloc(unsigned int order)
{
    if(order > MAX_ORDER){
        uart_puts("[__buddy_block_alloc] invalid order!\n");
        return 0;
    }
    
    uart_puts("[buddy allocate] **start**\n");
    for(int ord_ext=order; ord_ext<=MAX_ORDER; ord_ext++){
        if(list_empty(&free_list[ord_ext])){
            continue;
        }
        
        // block found, remove block from free list
        struct page *to_alloc = (struct page *)free_list[ord_ext].next;
        list_crop(&to_alloc->list, &to_alloc->list);
        to_alloc->order = order;

        // Release redundant memory block
        while(--ord_ext >= order){
            long buddy_pfn = FIND_BUDDY_PFN(to_alloc->pfn, ord_ext);
            struct page* buddy = &bookkeep[buddy_pfn];
            buddy->order = ord_ext;
            list_add_tail(&buddy->list, &free_list[ord_ext]);
            printf("\tfree redundant(PFN:%d, Order:%d)\n", buddy->pfn, buddy->order);
        }
        printf("\tallocated block(PFN:%d, Order:%d)\n[buddy allocate] **end**\n\n", to_alloc->pfn, to_alloc->order);
        return to_alloc;
    }
    uart_puts("[__buddy_block_alloc] No free space!\n");
    return 0;
}

void __buddy_block_free(struct page* block)
{
    long buddy_pfn, lbuddy_pfn, rbuddy_pfn;
    struct page *buddy, *lbuddy, *rbuddy;
    printf("[buddy free] **start**\n\tblock(PFN:%d, Order:%d)\n", block->pfn, block->order);
    block->used = 0;
    // coalesce free buddy
    buddy_pfn = FIND_BUDDY_PFN(block->pfn, block->order);
    buddy = &bookkeep[buddy_pfn];
    while(buddy->order == block->order && !buddy->used){
        list_crop(&buddy->list, &buddy->list);
        //merge
        printf("\tcoalesce free buddy(PFN:%d, Order:%d)\n", buddy->pfn, buddy->order);
        rbuddy_pfn = FIND_RBUDDY_PFN(block->pfn, block->order);
        rbuddy = &bookkeep[rbuddy_pfn];
        rbuddy->order = -1;
        lbuddy_pfn = FIND_LBUDDY_PFN(block->pfn, block->order);
        lbuddy = &bookkeep[lbuddy_pfn];
        lbuddy->order += 1;
        //next iteration
        block = lbuddy;
        buddy_pfn = FIND_BUDDY_PFN(block->pfn, block->order);
        buddy = &bookkeep[buddy_pfn];
    }
    list_add_tail(&block->list, &free_list[block->order]);
    uart_puts("[buddy free] **end**\n\n");
}