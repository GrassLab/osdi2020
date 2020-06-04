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
                struct buddy* temp = (struct buddy*)addr;
                temp->page_frame_number = page_order;
                temp->next = 0;
                if(j==0)
                    buddy_pool[order].page = temp;
                else{
                    struct buddy* head = buddy_pool[order].page;
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
        struct buddy* head = buddy_pool[order].page;

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

int buddy_alloc(int size){
    if(size > (1<<MAX_ORDER-1)){
        uart_puts("[ Allocate Fail! ]\n");
        uart_puts("Can't handle more than 1024 pages!\n");
        return -1;
    }

    int order = cal_order(size);
    uart_puts("[ Allocate pages ]\n");
    printf("Size: %d, Order: %d (page num: %d)\n", size, order, (1<<order));
    
    while(buddy_pool[order].len==0){
        int remain_order = order+1;
        while(buddy_pool[remain_order].len==0 && remain_order<MAX_ORDER)remain_order++;
        if(remain_order==MAX_ORDER){
                uart_puts("Allocate fail!\n");
                return -1;
        }
        
        // Split one buddy into two
        struct buddy* head = buddy_pool[remain_order].page;
        buddy_pool[remain_order].page = head->next;
        buddy_pool[remain_order].len--;

        int addr = head;
        addr+= (1 << (remain_order-1 + PAGE_SHIFT));     
        int big_page_size = (1<<remain_order-1);
        int big_page_num = head->page_frame_number;

        buddy_pool[remain_order-1].page = head;
        buddy_pool[remain_order-1].len += 2;
        struct buddy* temp = (struct buddy*)addr;
        temp->page_frame_number = big_page_num+big_page_size;
        temp->next = 0;
        head->next = temp;

        printf("Split for order: %d", remain_order);
        printf(" into %d and %d\n", big_page_num, big_page_num+big_page_size);
    }

    if(order<MAX_ORDER && buddy_pool[order].len>0){
        struct buddy* head = buddy_pool[order].page;
        int ret_page = head->page_frame_number;
        
        buddy_pool[order].page = head->next;
        buddy_pool[order].len--;
        printf("Allocate success in page frame number: %d", ret_page);
        printf(", physical address: %d", pfn2phy(ret_page));
        return ret_page;
    }
    uart_puts("Unknown error!");
    uart_puts("\n");
    return -1;
}

int pfn2phy(int page_frame_number){
    return LOW_MEMORY + (page_frame_number * PAGE_SIZE);
}

void check_merge(){
    for(int order=0; order<MAX_ORDER-1; order++){
        if(buddy_pool[order].len==0) continue;

        struct buddy* head = buddy_pool[order].page;
        while(head!=0 && head->next!=0){
            int buddy_num = (head->page_frame_number ^ (1<<order));

            if(buddy_num==head->next->page_frame_number){
                printf("Merge buddy: %d and %d\n", head->page_frame_number, head->next->page_frame_number);
                
                struct buddy* next_head = head->next->next;                
                // remove node for current order
                if(buddy_pool[order].page->page_frame_number == head->page_frame_number){
                    buddy_pool[order].page = next_head;
                }else{
                    struct buddy* pre_head = buddy_pool[order].page;
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
                        struct buddy* next_order_head = buddy_pool[order+1].page;
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

    struct buddy* temp = (struct buddy*)pfn2phy(page_frame_number);
    temp->page_frame_number = page_frame_number;
    temp->next = 0;

    if(buddy_pool[order].len==0){
        buddy_pool[order].page = temp;
    }else{
        struct buddy* head = buddy_pool[order].page;
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