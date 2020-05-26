#include "mm.h"

#include <stddef.h>

#include "printf.h"
#include "schedule.h"
#include "string.h"
#include "uart.h"


static struct page bookkeeping[NR_PAGE];
static char kstack_pool[NR_TASKS][THREAD_SIZE];
static char ustack_pool[NR_TASKS][THREAD_SIZE];

unsigned long get_kstack_base(unsigned long task_id){
    return (unsigned long)(kstack_pool[task_id]);
}

unsigned long get_ustack_base(unsigned long task_id){
    return (unsigned long)(ustack_pool[task_id]);
}

struct page* demand_page()
{
    struct page* page;
    for(int pfn=0; pfn<NR_PAGE; pfn++){
        if(bookkeeping[pfn].used)
            continue;
        page = &bookkeeping[pfn];
		memset((void *)page->phy_addr, 0, PAGE_SIZE);
        bookkeeping[pfn].used = 1;
        return page;
    }
    printf("[page_alloc] There are not free page!\n");
    return NULL;
}

void restore_page(struct page* page)
{
    page->used = 0;
}

void __init_page_struct()
{
    for(int i=0; i<NR_PAGE; i++){
        bookkeep[i].pfn = i;
        bookkeep[i].used = 0;
        bookkeep[i].phy_addr = LOW_MEMORY + i*PAGE_SIZE;
        bookkeep[i].order = -1;
        INIT_LIST_HEAD(&(bookkeep[i].list));// feature envy?
    }
}

void __init_buddy()
{
    for(int i=0; i<MAX_ORDER+1; i++){
        INIT_LIST_HEAD(&buddy_freelist[i]);
    }

    for(int i=0; i<NR_PAGE; i+=MAX_ORDER_SIZE){
        list_add_tail(&bookkeep[i].list, &buddy_freelist[MAX_ORDER]);
    }
}

#ifdef __DEBUG
void dump_buddy()
{
    uart_puts("\tbuddy_freelist={\n");
    for(int i=0; i<=MAX_ORDER; i++){
        printf("\t\tORDER_%d",i);
        list_head_t *next;
        for(next = buddy_freelist[i].next; next!=&buddy_freelist[i]; next = next->next){
            printf(" --> {pfn(%d)}", ((struct page *)next)->pfn);
        }
        uart_puts("\n");
    }
    uart_puts("\t}\n");
}
#endif//__DEBUG

struct page *__buddy_block_alloc(int order)
{
    if( (order>MAX_ORDER) | (order<0) ){
        uart_puts("[__buddy_block_alloc] invalid order!\n");
        return 0;
    }
    
    printf("[buddy allocate] order: %d\tsize: %d\n", order, 1<<order);
    #ifdef __DEBUG
    uart_puts("\tbefore\n");
    dump_buddy();
    #endif//__DEBUG
    for(int ord_ext=order; ord_ext<=MAX_ORDER; ord_ext++){
        if(list_empty(&buddy_freelist[ord_ext])){
            continue;
        }
        
        // block found, remove block from free list
        struct page *to_alloc = (struct page *)buddy_freelist[ord_ext].next;
        list_crop(&to_alloc->list, &to_alloc->list);
        to_alloc->order = order;
        to_alloc->used = 1;

        // Release redundant memory block
        while(--ord_ext >= order){
            long buddy_pfn = FIND_BUDDY_PFN(to_alloc->pfn, ord_ext);
            struct page* buddy = &bookkeep[buddy_pfn];
            buddy->order = ord_ext;
            list_add_tail(&buddy->list, &buddy_freelist[ord_ext]);
            printf("\tfree redundant(PFN:%d, Order:%d)\n", buddy->pfn, buddy->order);
        }
        printf("\tallocated block(PFN:%d, Order:%d)\n", to_alloc->pfn, to_alloc->order);
        #ifdef __DEBUG   
        uart_puts("\tafter\n");
        dump_buddy();
        #endif//__DEBUG
        uart_puts("[buddy allocate] **done**\n\n");
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
    #ifdef __DEBUG
    uart_puts("before\n");
    dump_buddy();
    #endif//__DEBUG
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
        lbuddy_pfn = FIND_LBUDDY_PFN(block->pfn, block->order);
        lbuddy = &bookkeep[lbuddy_pfn];
        rbuddy->order = -1;
        lbuddy->order += 1;
        //next iteration
        block = lbuddy;
        buddy_pfn = FIND_BUDDY_PFN(block->pfn, block->order);
        buddy = &bookkeep[buddy_pfn];
    }
    list_add_tail(&block->list, &buddy_freelist[block->order]);
    #ifdef __DEBUG
    uart_puts("after\n");
    dump_buddy();
    #endif//__DEBUG
    uart_puts("[buddy free] **done**\n\n");
}

void __init_obj_alloc(struct obj_alloc *alloc, unsigned int size)
{
    alloc->curr_page = NULL;
    INIT_LIST_HEAD(&alloc->partial);
    INIT_LIST_HEAD(&alloc->full);
    INIT_LIST_HEAD(&alloc->empty);
    alloc->objsize = size;
}

int register_obj_allocator(unsigned int objsize)
{
    if(objsize<MIN_ALLOCATOR_SIZE){
        objsize = MIN_ALLOCATOR_SIZE;
        uart_puts("[register_obj_allocator] reset objsize to MIN_ALLOCATOR_SIZE\n");
    }
    for(int token=0; token<MAX_ALLOCATOR_NUM; token++){
        if(allocator_pool[token].objsize)
            continue;
        __init_obj_alloc(&allocator_pool[token], objsize);
        #ifdef __DEBUG
        printf("[register_obj_allocator] successed!\n\tobjsize: %d\ttoken: %d\n\n",objsize, token);
        #endif//__DEBUG
        return token;
    }
    uart_puts("[register_obj_allocator] Allocator pool has fulled\n");
    return -1;
}

void __init_obj_page(struct page* page, unsigned size)
{
    page->nr_unused = page->nr_obj = PAGE_SIZE / size;

    unsigned long chunk_header = page->phy_addr + size*(page->nr_obj-1);
    page->obj_freelist = (void **)chunk_header;
    while(chunk_header > page->phy_addr){
        *(void **)chunk_header = (void *)(chunk_header - size);
        chunk_header -= size;
    }
    *(void **)chunk_header = NULL;
    #ifdef __DEBUG
    printf("[__init_obj_page] chunk_size: %d\tnr_chunk: %d\n",size, page->nr_obj);
    unsigned long base = page->phy_addr;
    for(int i=0; i<3;i++){
        printf("\tchunk_%d @ 0x%X --> (void *)0x%X\n", i, (base+i*size), *(unsigned long *)(base+i*size));
    }
    uart_puts("\t\t---skip---\n");
    for(int i=page->nr_obj-3; i<page->nr_obj;i++){
        printf("\tchunk_%d @ 0x%X --> (void *)0x%X\n", i, (base+i*size), *(unsigned long *)(base+i*size));
    }
    printf("\tpage->obj_freelist  --> (void *)0x%X\n",(unsigned long)page->obj_freelist);

    uart_puts("[__init_obj_page] end\n\n");
    #endif//__DEBUG    
}

#ifdef __DEBUG
void dump_alloc(struct obj_alloc* alloc){
    list_head_t *list;
    uart_puts("\t\tobj_alloc = {\n");
    printf("\t\talloc.objsize=%d\n", alloc->objsize);
    printf("\t\talloc.curr_page{pfn=%d}\n", (alloc->curr_page == 0) ? -1 : alloc->curr_page->pfn);
    printf("\t\t - nr_unused: %d\n", (alloc->curr_page == 0) ? -1 : alloc->curr_page->nr_unused);
    list =  alloc->partial.next;
    uart_puts("\t\talloc.partial");
    while(list != &alloc->partial){
        printf("--> {pfn=%d}", ((struct page*)list)->pfn);
        list = list->next;
    }
    list =  alloc->full.next;
    uart_puts("\n\t\talloc.full");
    while(list != &alloc->full){
        printf("--> {pfn=%d}", ((struct page*)list)->pfn);
        list = list->next;
    }
    list =  alloc->empty.next;
    uart_puts("\n\t\talloc.empty");
    while(list != &alloc->empty){
        printf("--> {pfn=%d}", ((struct page*)list)->pfn);
        list = list->next;
    }
    uart_puts("\n}\n");
}
#endif//__DEBUG

void *obj_allocate(int token)
{
    if(token < 0 || token >= MAX_ALLOCATOR_NUM){
        uart_puts("[obj allocator] invalid token\n");
        return NULL;
    }
    struct obj_alloc* alloc = &allocator_pool[token];
    #ifdef __DEBUG
    printf("[obj_allocate] token: %d\tsize: %d\n",token, alloc->objsize);
    uart_puts("\tbefore allocation\n");
    dump_alloc(alloc);
    #endif//__DEBUG
    
    if(alloc->curr_page == NULL){
        struct page *page;
        if(!list_empty(&alloc->partial)){// find chunk from previous page
            page = (struct page*)alloc->partial.next;
            list_crop(&page->list, &page->list);
        }else if(!list_empty(&alloc->empty)){// using empty object page as second choice
            page = (struct page*)alloc->empty.next;
            list_crop(&page->list, &page->list);
        }else{// demamd new page
            page = __buddy_block_alloc(0);
            __init_obj_page(page, alloc->objsize);
            page->obj_alloc = alloc;
        }
        alloc->curr_page = page;
    }
    //allocate object
    struct page *curr_page = alloc->curr_page;
    void *obj = (void *)curr_page->obj_freelist;
    curr_page->obj_freelist = *curr_page->obj_freelist;
    //check full
    if(-- curr_page->nr_unused == 0){
        list_add_tail((list_head_t *)curr_page, &alloc->full);
        alloc->curr_page = NULL;
    }
    #ifdef __DEBUG
    uart_puts("\tafter allocation\n");
    dump_alloc(alloc);
    uart_puts("[obj_allocate] **done**\n\n");
    #endif//__DEBUG
    return obj;
}

void obj_free(void *obj)
{   
    int pfn = PHY_ADDR_TO_PFN(obj);
    struct page* page = &bookkeep[pfn];
    struct obj_alloc* alloc = page->obj_alloc;
    #ifdef __DEBUG
    printf("[obj_free] 0x%X (pfn=%d)\n", obj, pfn);
    printf("\tbefore free\n");
    printf("\t\tfreelist --> (0x%X)\n", page->obj_freelist);
    dump_alloc(alloc);
    printf("\t\tnr_unused: %d\n", page->nr_unused);
    #endif//__DEBUG
    // add to freelist
    void **header = (void **)obj;
    *header = (void *)page->obj_freelist;
    page->obj_freelist = header;
    page->nr_unused++;
    
    // from full to partial
    if(page->nr_unused == 1){
        list_crop(&page->list, &page->list);
        list_add_tail(&page->list, &alloc->partial);
    }
    // from partial to empty
    if(page->nr_unused == page->nr_obj){
        list_crop(&page->list, &page->list);
        //keep one empty page to prevent frequentlly page demand
        if(list_empty(&alloc->empty)){
            list_add_tail(&page->list, &alloc->empty);
        }else{
            page->obj_alloc = NULL;
            __buddy_block_free(page);
        }
    }
    #ifdef __DEBUG
    printf("\tafter free\n");
    printf("\t\tfreelist --> (0x%X) --> (0x%X)\n", page->obj_freelist, *page->obj_freelist);
    dump_alloc(alloc);
    printf("\t\tnr_unused: %d\n", page->nr_unused);
    uart_puts("[obj_free] **done**\n\n");
    #endif//__DEBUG
}

void __init_kmalloc()
{
    for (unsigned int i=MIN_KMALLOC_ORDER; i<=MAX_KMALLOC_ORDER; i++){
        register_obj_allocator(1<<i);
    }
}

void *kmalloc(unsigned int size)
{
    #ifdef __DEBUG
    printf("[kmalloc] size: %d\n", size);
    #endif//__DEBUG
    void *block;
    for(unsigned int i=MIN_KMALLOC_ORDER; i<MAX_KMALLOC_ORDER; i++){
        if(size <= (1<<i)){
            block = obj_allocate(i-MIN_KMALLOC_ORDER);
            #ifdef __DEBUG
            printf("[kmalloc] **done** block@ 0x%X\n\n", block);
            #endif//__DEBUG
            return block;
        }
    }
    for(unsigned int i=0; i<MAX_ORDER; i++){
        if(size <= 1<<(i+PAGE_SHIFT)){
            block = (void *)(__buddy_block_alloc(i)->phy_addr);
            #ifdef __DEBUG
            printf("[kmalloc] **done** block@ 0x%X\n\n", block);
            #endif//__DEBUG
            return block;
        }
    }
    uart_puts("[kmalloc] too large!\n");
    return NULL;
}

void kfree(void * block)
{
    #ifdef __DEBUG
    printf("[kfree] block@ 0x%X\n", block);
    #endif//__DEBUG
    int pfn = PHY_ADDR_TO_PFN(block);
    struct page* page = &bookkeep[pfn];
    if(page->obj_alloc)
        obj_free(block);
    else
        __buddy_block_free(page);
    #ifdef __DEBUG
    printf("[kfree] **done**\n\n");
    #endif//__DEBUG
}

void mm_init()
{
    __init_page_struct();// must do before buddy or it will break the list
    __init_buddy();
    __init_kmalloc();
    // __init_obj_allocator; // init bss should set them to size 0(inused)
}
