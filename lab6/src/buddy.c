#include "buddy.h"
#include "tools.h"
int cal_order(int size) {
  int targetlevel = 0;
  int add1_flag = 0;
  while (size >> 1) {

    if (size & 1)
      add1_flag = 1;

    ++targetlevel;
    size >>= 1;
  }

  return targetlevel + add1_flag;
}
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
int buddy_alloc(int size){
    
    uart_puts("Allocate for size: ");
    uart_send_int(size);
    uart_puts("\n");

    if(size > (1<<MAX_ORDER-1)){
        uart_puts("Can't handle such size!");
        uart_puts("\n");
        return -1;
    }

    int order = cal_order(size);
    
    while(buddy_pool[order].len==0){
        int remain_order = order+1;
        while(buddy_pool[remain_order].len==0 && remain_order<MAX_ORDER)remain_order++;
        if(remain_order==MAX_ORDER){
            uart_puts("Allocate fail!");
            uart_puts("\n");
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

        // uart_puts("Split for order: ");
        // uart_send_int(remain_order);
        // uart_puts(" in: ");
        // uart_send_int(big_page_num);
        // uart_puts("\n");
    }

    if(order<MAX_ORDER && buddy_pool[order].len>0){
        struct buddy* head = buddy_pool[order].page;
        int ret_page = head->page_frame_number;
        
        buddy_pool[order].page = head->next;
        buddy_pool[order].len--;
        uart_puts("Allocate success in: ");
        uart_send_int(ret_page);
        uart_puts(" physical address: ");
        uart_hex(pfn2phy(ret_page));
        uart_puts(" size: ");
        uart_send_int((1<<order));
        uart_puts("\n");
        return ret_page;
    }
}
void buddy_show(){
    for(int order=0;order<MAX_ORDER;order++){

        // if(buddy_pool[order].len==0)continue;
        uart_puts("Order: ");
        uart_send_int(order);
        uart_puts("\n");
        uart_puts("Len: ");
        uart_send_int(buddy_pool[order].len);
        uart_puts("\n");
        struct buddy* head = buddy_pool[order].page;

        if(head==0){
            uart_puts("\n");
            continue;
        }
        while(head != 0){
            uart_send_int(head->page_frame_number);
            head = head->next;
            uart_puts("->");
        }
        uart_puts("NULL\n\n");
    }
}
int pfn2phy(int page_frame_number){
    return LOW_MEMORY + (page_frame_number * PAGE_SIZE);
}