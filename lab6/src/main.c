#include <stddef.h>
#include <stdint.h>

#include "tools.h"
#include "printf.h"

#include "mailbox.h"


void test_buddy(){
    buddy_init(PAGING_PAGES);
    buddy_show();

    // int alloc_num = 11;
    // int alloc_size[] = {34, 66, 35, 67, 1024, 4, 16, 17, 257, 53, 0};
    int alloc_num = 5;
    int alloc_size[] = {34, 66, 35, 67, 1024};
    int alloced_pool[alloc_num];
    for(int i=0;i<alloc_num;i++){
        alloced_pool[i] = buddy_alloc(alloc_size[i]);
        buddy_show();
    }

    for(int i=0;i<alloc_num;i++){
        if(alloced_pool[i]==-1)continue;
        buddy_free(alloced_pool[i], alloc_size[i]);
        buddy_show();
    }

}
int alloc_num = 5;
int chunk_size1 = 1024;
int chunk_size2 = 16;
int chunk_size3 = 16;

void test_slab(){
    buddy_init(PAGING_PAGES);    
    int allocator_id = init_allocator(chunk_size1);

    int addrs[alloc_num];
    for(int i=0;i<alloc_num;i++){
        show_allocator(allocator_id);
        addrs[i] = alloc(allocator_id);
        show_allocator(allocator_id);

        uart_puts("\n");
        uart_puts("addr: ");
        uart_hex(addrs[i]);
        uart_puts("\n");
    }
    for(int i=0;i<alloc_num;i++){
        show_allocator(allocator_id);
        free_alloc(allocator_id, addrs[i]);        
    }
    show_allocator(allocator_id);
    
}
void main(){
    // set up serial console
    uart_init();
    //init_printf(NULL, putc);
    //mmu_init();

    // get_vc_information();
    // get_arm_information();
    // get_serial();
    // get_board_revision();
    // call_sys_enable_time();
    
     
    
    //init_init_task(shell);
    // extern unsigned long _binary_user_code_test_img_start;
    // unsigned long program_start = (unsigned long)&_binary_user_code_test_img_start;
    // echo everything back
    //privilege_task_create(program_start);
    //schedule();
    //print_hello();
    
    //uart_puts("# ");
    //shell();


    uart_puts("LOW_MEMORY: ");
    uart_hex(LOW_MEMORY);
    uart_puts("\n");
    uart_puts("HIGH_MEMORY: ");
    uart_hex(HIGH_MEMORY);
    uart_puts("\n");
    uart_puts("PAGING_MEMORY: ");
    uart_send_int(PAGING_MEMORY);
    uart_puts("\n");
    uart_puts("PAGING_PAGES: ");
    uart_send_int(PAGING_PAGES);
    uart_puts("\n");

    // test_buddy();
    test_slab();
    shell();
}	
