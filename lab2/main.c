#include "tools.h"
#include "mailbox.h"

void main(){
    // set up serial console
    uart_init();
    
    get_board_revision();
    get_serial();
    print_hello();    
    uart_puts("# ");
    char *command = "";
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