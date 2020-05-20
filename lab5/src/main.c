#include "tools.h"
#include "mailbox.h"

void main(){
    // set up serial console
    uart_init();

    mmu_init();

    // get_vc_information();
    // get_arm_information();
    // get_serial();
    // get_board_revision();
    call_sys_enable_time();
    
    print_hello();    
    uart_puts("# ");
    uart_send_int(remain_page_num());  
    init_init_task(shell);
    uart_send_int(remain_page_num());
    
    // echo everything back
    shell();
   
}	
