#include "tools.h"
#include "mailbox.h"


void main(){
    // set up serial console
    uart_init();
    
    get_vc_information();
    get_arm_information();
    get_serial();
    get_board_revision();

    print_hello();    
    uart_puts("# ");
            
    init_init_task(shell);
    
    // echo everything back
    shell();
    
}	