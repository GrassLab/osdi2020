#include "uart.h"
#include "reset.h"

int my_strcmp(char * str1, char * str2){
    int i=0;    
    while(str1[i]!='\0'){        
        if(str1[i] != str2[i])return 0;        
        i++;
    }
    if(str2[i]!='\0')
        return 0;
    return 1;
}
int cmd_box(char * command){
    if(my_strcmp(command, "help")==1)return 1;
    if(my_strcmp(command, "hello")==1)return 2;
    if(my_strcmp(command, "reset")==1)return 3;
    return -1;
}
void process_cmd(char * command){
    switch (cmd_box(command)){
        case 1:
            uart_puts("help        :show every commands.\n");
            uart_puts("hello       :say hello to you.\n");
            uart_puts("reset       :reboot the mechine.\n");
            break;
        case 2:
            uart_puts("Hello World!");
            break;
        case 3:
            uart_puts("Rebooting");
            my_reset();
            break;
        default:
            uart_puts(command);
            uart_puts(": command not found");
            break;
    }
    command[0] = '\0';
}
void print_hello(){
    // say hello
    uart_puts("  _____ ____    _    ___ _        ___  ____   \n");
    uart_puts(" |_   _/ ___|  / \\  |_ _( )___   / _ \\/ ___|  \n");
    uart_puts("   | | \\___ \\ / _ \\  | ||// __| | | | \\___ \\  \n");
    uart_puts("   | |  ___) / ___ \\ | |  \\__ \\ | |_| |___) | \n");
    uart_puts("   |_| |____/_/   \\_\\___| |___/  \\___/|____/  \n");
                                                              
    uart_puts("     ___           ___           ___       ___       ___        \n");
    uart_puts("    /\\__\\         /\\  \\         /\\__\\     /\\__\\     /\\  \\       \n");
    uart_puts("   /:/  /        /::\\  \\       /:/  /    /:/  /    /::\\  \\      \n");
    uart_puts("  /:/__/        /:/\\:\\  \\     /:/  /    /:/  /    /:/\\:\\  \\     \n");
    uart_puts(" /::\\  \\ ___   /::\\~\\:\\  \\   /:/  /    /:/  /    /:/  \\:\\  \\    \n");
    uart_puts("/:/\\:\\  /\\__\\ /:/\\:\\ \\:\\__\\ /:/__/    /:/__/    /:/__/ \\:\\__\\   \n");
    uart_puts("\\/__\\:\\/:/  / \\:\\~\\:\\ \\/__/ \\:\\  \\    \\:\\  \\    \\:\\  \\ /:/  /   \n");
    uart_puts("     \\::/  /   \\:\\ \\:\\__\\    \\:\\  \\    \\:\\  \\    \\:\\  /:/  /    \n");
    uart_puts("     /:/  /     \\:\\ \\/__/     \\:\\  \\    \\:\\  \\    \\:\\/:/  /     \n");
    uart_puts("    /:/  /       \\:\\__\\        \\:\\__\\    \\:\\__\\    \\::/  /      \n");
    uart_puts("    \\/__/         \\/__/         \\/__/     \\/__/     \\/__/       \n");

}
void main(){
    // set up serial console
    uart_init();
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