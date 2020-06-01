#include <stddef.h>
#include <stdint.h>

#include "tools.h"
#include "printf.h"

#include "mailbox.h"
void task_program()
{
    extern unsigned long _binary_user_code_test_img_start;
    extern unsigned long _binary_user_code_test_img_size;
    unsigned long program_start = (unsigned long)&_binary_user_code_test_img_start;
    unsigned long program_size = (unsigned long)&_binary_user_code_test_img_size;

    //printf("\ntask_id: %d\n", get_taskid());

    call_exec(program_start, program_size);
    
}
struct node{
    int a;
};

void test_buddy(){
    buddy_init(PAGING_PAGES);
    buddy_show();

    int alloc_num = 10;
    int alloced_pool[alloc_num];
    int alloc_size[] = {34, 66, 35, 67, 1024, 4, 16, 17, 257, 53};
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
void main(){
    // set up serial console
    uart_init();
    init_printf(NULL, putc);
    //mmu_init();

    // get_vc_information();
    // get_arm_information();
    // get_serial();
    // get_board_revision();
    call_sys_enable_time();
    
     
    
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
    test_buddy();
    
    shell();
}	
void run_program()
{
    extern unsigned long _binary_user_code_test_img_start;
    unsigned long program_start = (unsigned long)&_binary_user_code_test_img_start;

    ((void (*)(void))program_start)();
}

