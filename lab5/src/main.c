#include "tools.h"
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
void main(){
    // set up serial console
    uart_init();

    mmu_init();

    // get_vc_information();
    // get_arm_information();
    // get_serial();
    // get_board_revision();
    call_sys_enable_time();
    
     
    
    init_init_task(shell);
    extern unsigned long _binary_user_code_test_img_start;
    unsigned long program_start = (unsigned long)&_binary_user_code_test_img_start;
    // echo everything back
    privilege_task_create(program_start);
    schedule();
    // print_hello();   
    // uart_puts("# ");
    // shell();
   
}	
void run_program()
{
    extern unsigned long _binary_user_code_test_img_start;
    unsigned long program_start = (unsigned long)&_binary_user_code_test_img_start;

    ((void (*)(void))program_start)();
}

