#include "uart.h"

int do_hello_cmd(void) 
{
    uart_puts("Hello World!\n");
    return 1;
}

int do_help_cmd(void) 
{
    uart_puts("help : print all available commands\r");
    uart_puts("hello : print Hello World!\r");
    uart_puts("timestamp : print current timestamp.\r");
    uart_puts("reboot : reboot.\n");
    return 1;
}
int sstrcmp(char *array_1, char *array_2) {
    int i = 0;
    while(array_1[i] != '\0') {
        if (array_1[i] != array_2[i]) {
            return -1;
        }
        i++;
    }
    if(array_2[i] != '\0') {
        return -1;
    }
    return 0;
}

int process_cmd(char *command)
{
    if(!(sstrcmp(command, "hello"))) {
        do_hello_cmd();
        return 1;
    }
    if(!(sstrcmp(command, "help"))) {
        do_help_cmd();
        return 1;    
    }
    return 0;
}

void main()
{
    // set up serial console
    uart_init();
    
    // say hello
    uart_puts("Hello World!\n");
    
    char command_buffer[51];
    char *ptr_cmd = command_buffer;
    // echo everything back
    while(1) {
	    char g = uart_getc();
        if (g != '\n') {
            *ptr_cmd++ = g;
        }
        else {
            *ptr_cmd = '\0';
            if (!process_cmd(command_buffer)) {
                uart_puts("process command error\n");
            }
            ptr_cmd = command_buffer;
        }
    }
}
