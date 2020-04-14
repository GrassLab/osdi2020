#include "tools.h"
#include "mailbox.h"

char command[100];

void main(){
    // set up serial console
    uart_init();
    
    get_vc_information();
    get_arm_information();
    get_serial();
    get_board_revision();
    
    init_irq ();
    enable_irq ();

    print_hello();    
    uart_puts("# ");
            
    int command_index=0;
    // echo everything back
    while(1) {
        char in = uart_getc();
        if(in!='\n'){            
            if((unsigned int)in>=32 && (unsigned int)in<=126)
            command[command_index++] = in;
            uart_send(in);
        }
        else if(in == 8 || in ==46){
            uart_send('\b');
            command_index--;
        }
        else{
            command[command_index] = '\0';            
            if(command_index>0){
                uart_puts("\n");
                process_cmd(command);
            }
            uart_puts("\n# ");
            command_index = 0;
        }
        
    }
}	