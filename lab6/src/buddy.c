#include "buddy.h"
#include "tools.h"

void buddy_init(int num_pages){
    unsigned long addr = LOW_MEMORY;
    int page_order = 0;

    for (int i=0;i<MAX_ORDER;i++){
        buddy_pool[i].len = 0;
        buddy_pool[i].page = 0;
    }
    
    for (int order=MAX_ORDER-1;order>=0;order--){
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
                    while(head->next!=0)head = head->next;
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
    for(int order=0;order<MAX_ORDER;order++){
        uart_send_int(buddy_pool[order].len);
        uart_puts("\n");
        struct buddy* head = buddy_pool[order].page;
        if(head==0)continue;
        while(head != 0){
            uart_send_int(head->page_frame_number);
            head = head->next;
            uart_puts("->");
        }
        uart_puts("NULL\n");
    }
}