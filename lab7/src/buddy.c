#include "buddy.h"
#include "mm.h"
#include "printf.h"
#include "uart.h"

int cal_order(int size) {
    int targetlevel = 0;
    int add1_flag = 0;
    while (size >> 1) {
        if (size & 1){
            add1_flag = 1;
        }
        ++targetlevel;
        size >>= 1;
    }
    return targetlevel + add1_flag;
}

void buddy_init(int num_pages){
    unsigned long addr = LOW_MEMORY;
    int page_order = 0;

    for(int i=0; i<MAX_ORDER; i++){
        buddy_pool[i].len = 0;
        buddy_pool[i].page = 0;
    }

    for(int order=MAX_ORDER-1; order>=0; order--){
        printf("number of pages: %d\n", num_pages);
        int page_in_order = num_pages/(1<<order);
        if(page_in_order>0){
            buddy_pool[order].len = page_in_order;
            for(int j=0;j<page_in_order;j++){           
                struct page* temp = (struct page*)addr;
                temp->page_frame_number = page_order;
                temp->next = 0;
                if(j==0)
                    buddy_pool[order].page = temp;
                else{
                    struct page* head = buddy_pool[order].page;
                    while(head->next)head = head->next;
                    head->next = temp;
                }
                addr+= (1 << (order + PAGE_SHIFT));                
                page_order+=(1 << order);
            }
        }
        num_pages = num_pages%(1<<order);
    }

}

void buddy_show(){
    uart_puts("\n");
    uart_puts("[ Show Buddy ]\n");

    for(int order=0;order<MAX_ORDER;order++){
        printf("Order: %d (page num: %d)", order, (1<<order));
        printf(", Length: %d\n", buddy_pool[order].len);
        struct page* head = buddy_pool[order].page;

        if(head==0){
                continue;
        }
        while(head != 0){
            printf("%d -> ",head->page_frame_number);
                head = head->next;
        }
        uart_puts("NULL\n");
    }
    uart_puts("\n");
}

struct page *buddy_alloc(int size){
    if(size > (1<<(MAX_ORDER-1))){
        uart_puts("[ Allocate Fail! ]\n");
        uart_puts("Can't handle more than 1024 pages!\n");
        return 0;
    }

    int order = cal_order(size);
    uart_puts("[ Allocate pages ]\n");
    printf("Size: %d, Order: %d (page num: %d)\n", size, order, (1<<order));
    
    while(buddy_pool[order].len==0){
        int remain_order = order+1;
        while(buddy_pool[remain_order].len==0 && remain_order<MAX_ORDER)remain_order++;
        if(remain_order==MAX_ORDER){
                uart_puts("Allocate fail!\n");
                return 0;
        }
        
        // Split one page into two
        struct page* head = buddy_pool[remain_order].page;
        buddy_pool[remain_order].page = head->next;
        buddy_pool[remain_order].len--;

        int addr = head;
        addr+= (1 << (remain_order-1 + PAGE_SHIFT));     
        int big_page_size = (1<<(remain_order-1));
        int big_page_num = head->page_frame_number;

        buddy_pool[remain_order-1].page = head;
        buddy_pool[remain_order-1].len += 2;
        struct page* temp = (struct page*)addr;
        temp->page_frame_number = big_page_num+big_page_size;
        temp->next = 0;
        head->next = temp;

        printf("Split for order: %d", remain_order);
        printf(" into %d and %d\n", big_page_num, big_page_num+big_page_size);
    }

    if(order<MAX_ORDER && buddy_pool[order].len>0){
        struct page* head = buddy_pool[order].page;
        int ret_page = head->page_frame_number;
        head->physical_addr = pfn2phy(ret_page);
        head->size = (1<<order);
        
        buddy_pool[order].page = head->next;
        buddy_pool[order].len--;
        printf("Allocate success in page frame number: %d", ret_page);
        printf(", physical address: %d\n", head->physical_addr);
        return head;
    }
    uart_puts("Unknown error!");
    uart_puts("\n");
    return 0;
}

int buddy_alloc_ret_pfn(int size){
    struct page *head;
    head = buddy_alloc(size);
    if (head == 0){
        return -1;
    }
    return head->page_frame_number;
}

int pfn2phy(int page_frame_number){
    return LOW_MEMORY + (page_frame_number * PAGE_SIZE);
}

int phy2pfn(int physical_addr){
    return (((((unsigned long)(physical_addr)) - LOW_MEMORY) & 0x0000FFFFFFFFF000) >> PAGE_SHIFT);
}

void check_merge(){
    for(int order=0; order<MAX_ORDER-1; order++){
        if(buddy_pool[order].len==0) continue;

        struct page* head = buddy_pool[order].page;
        while(head!=0 && head->next!=0){
            int buddy_num = (head->page_frame_number ^ (1<<order));

            if(buddy_num==head->next->page_frame_number){
                printf("Merge page: %d and %d\n", head->page_frame_number, head->next->page_frame_number);
                
                struct page* next_head = head->next->next;                
                // remove node for current order
                if(buddy_pool[order].page->page_frame_number == head->page_frame_number){
                    buddy_pool[order].page = next_head;
                }else{
                    struct page* pre_head = buddy_pool[order].page;
                    while(pre_head->next->page_frame_number != head->page_frame_number)pre_head = pre_head->next;                    
                    pre_head->next = next_head;
                }

                // find position in next order
                if(buddy_pool[order+1].len==0){
                    buddy_pool[order+1].page = head;
                    head->next = 0;
                }else{
                    if(head->page_frame_number < buddy_pool[order+1].page->page_frame_number){                        
                        head->next = buddy_pool[order+1].page;
                        buddy_pool[order+1].page = head;                        
                    }
                    else{
                        struct page* next_order_head = buddy_pool[order+1].page;
                        while(next_order_head->next!=0 && next_order_head->next->page_frame_number < head->page_frame_number)next_order_head = next_order_head->next;                        
                        head->next = next_order_head->next;
                        next_order_head->next = head;
                    }
                }   
                
                buddy_pool[order].len -= 2;
                buddy_pool[order+1].len += 1;
                head = next_head;
            }else{
                head = head->next;
            } 
        }
    }
}

void buddy_free(int page_frame_number, int page_frame_size){
    int order = cal_order(page_frame_size);
    uart_puts("[ Free Pages ]\n");
    printf("Size: %d, Page num: %d\n", page_frame_size, (1<<order));
    printf("Page Frame number: %d\n", page_frame_number);

    struct page* temp = (struct page*)pfn2phy(page_frame_number);
    temp->page_frame_number = page_frame_number;
    temp->next = 0;

    if(buddy_pool[order].len==0){
        buddy_pool[order].page = temp;
    }else{
        struct page* head = buddy_pool[order].page;
        if(page_frame_number < head->page_frame_number){
            temp->next = head;
            buddy_pool[order].page = temp;            
        }else{
            while(head->next!=0 && head->next->page_frame_number < page_frame_number)head = head->next;
            temp->next = head->next;
            head->next = temp;
        }
    }
    buddy_pool[order].len++;
    check_merge();
}

void _buddy_free(int page_frame_number, void *block){
    struct page *p = (struct page*)block;
    buddy_free(page_frame_number, p->size);
}

/////////////////////// obj allocate ///////////////////////

void __init_obj_alloc(struct obj_alloc *alloc, unsigned int size){
    alloc->curr_page = 0; // NULL
    alloc->used_page = 0; // NULL
    alloc->obj_size = size;
    alloc->used = 1;
}

int register_obj_allocator(unsigned int obj_size){
    if(obj_size<MIN_ALLOCATOR_SIZE){
        obj_size = MIN_ALLOCATOR_SIZE;
        uart_puts("[register_obj_allocator] reset objsize to MIN_ALLOCATOR_SIZE\n");
    }
    for(int index=0; index<MAX_ALLOCATOR_NUMBER; index++){
        if(allocator_pool[index].used)
            continue;
        
        __init_obj_alloc(&allocator_pool[index], obj_size);
        return index;
    }
    uart_puts("[register_obj_allocator] Allocator pool has fulled\n");
    return -1;
}

void *obj_allocate(int token){
    if(token < 0 || token >= MAX_ALLOCATOR_NUMBER){
        uart_puts("[obj allocator] invalid token\n");
        return 0;
    }

    struct obj_alloc* alloc = &allocator_pool[token];

    if(alloc->curr_page == 0){
        struct page *page;
        int check = 0;
        if(alloc->used_page != 0){
            page = alloc->used_page;
            while(page->unused_obj == 0){
                if(page->next == 0) break;
                page = page->next;
            }
            if(page->unused_obj > 0){
                check = 1;
                printf("pfn: %d, unused_obj: %d\n", page->page_frame_number, page->unused_obj);
            }
        }
        
        if(check == 1){
            alloc->curr_page = page;
        }else{// demamd new page
            page = buddy_alloc(1);
            __init_obj_page(page, alloc->obj_size);
            page->obj_alloc = alloc;

            alloc->curr_page = page;
            page->next = alloc->used_page;
            alloc->used_page = page;
            // printf("phy of next page: %d\n", page->next);
        }
        
        printf("[obj allocator] pfn: %d, phy: %d\n", alloc->curr_page->page_frame_number, alloc->curr_page->physical_addr);
    }
    //allocate object
    struct page *curr_page = alloc->curr_page;
    void *obj = (void *)curr_page->obj_freelist;
    curr_page->obj_freelist = *curr_page->obj_freelist;
    printf("[allocate obj] size: %d, physical addr: %d\n", alloc->obj_size, obj);

    // check full
    (curr_page->unused_obj)--;
    if(curr_page->unused_obj == 0){
        alloc->curr_page = 0;   
    }
    return obj;
}

void __init_obj_page(struct page* page, unsigned size)
{
    page->unused_obj = PAGE_SIZE / size;

    unsigned long chunk_header = page->physical_addr + size*(page->unused_obj-1);
    page->obj_freelist = (void **)chunk_header;
    while(chunk_header > page->physical_addr){
        *(void **)chunk_header = (void *)(chunk_header - size);
        chunk_header -= size;
    }
    *(void **)chunk_header = 0;
    return;
}

void _obj_free(void *obj, struct page *page)
{   
    // add to freelist
    void **header = (void **)obj;
    *header = (void *)page->obj_freelist;
    page->obj_freelist = header;
    page->unused_obj++;

    printf("[free obj] pfn: %d, unused num: %d\n", page->page_frame_number, page->unused_obj);
    return;
}

void obj_free(void *obj, int token)
{
    if(token < 0 || token >= MAX_ALLOCATOR_NUMBER){
        uart_puts("[obj allocator] invalid token\n");
        return;
    }

    struct obj_alloc* alloc = &allocator_pool[token];
    int pfn = phy2pfn((int *)obj);

    struct page* page = alloc->used_page;
    while(pfn != page->page_frame_number){
        if(page->next == 0) break;
        page = page->next;
    }

    _obj_free(obj, page);
}

/////////////////////// kmalloc ///////////////////////
void __init_kmalloc()
{
    for (unsigned int i=MIN_KMALLOC_ORDER; i<=MAX_KMALLOC_ORDER; i++){
        register_obj_allocator(1<<i);
    }
}

void *kmalloc(unsigned int size)
{
    void *block;
    // size <= 2048 bytes
    for(unsigned int i=MIN_KMALLOC_ORDER; i<MAX_KMALLOC_ORDER; i++){
        if(size <= (1<<i)){
            block = obj_allocate(i-MIN_KMALLOC_ORDER);
            return block;
        }
    }

    // size <= 1024 pages
    int x = (size / (1<<PAGE_SHIFT))+1;
    if(x<(1<<MAX_ORDER)){
        block = (void *)(buddy_alloc(x)->physical_addr);
        return block;
    }

    uart_puts("[kmalloc] too large!\n");
    return 0;
}

void kfree(void *block)
{
    int pfn = phy2pfn(block);
    struct page* page = find_page(pfn);
    if(page == 0){
        _buddy_free(pfn, block);
    }else{
        _obj_free(block, page);
    }
    return;
}

struct page *find_page(int pfn)
{
    struct obj_alloc* alloc;
    struct page* page;
    for(int token=0; token<MAX_ALLOCATOR_NUMBER-MIN_ALLOCATOR_SIZE+1; token++){
        alloc = &allocator_pool[token];
        if(alloc->used_page!=0){
            page = alloc->used_page;
            while(pfn != page->page_frame_number){
                if(page->next == 0) break;
                page = page->next;
            }
            if (pfn == page->page_frame_number){
                // printf("%d\n", page->page_frame_number);
                return page;
            }
        }
    }
    return 0;
}