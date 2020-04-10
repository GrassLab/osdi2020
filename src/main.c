#include "uart.h"
#include "shell.h"

int main()
{
    // set up serial console
    uart_init();

    // say hello
    uart_printf("Hello World\n\n");

    // start shell
    shell_start();
    
    return 0;
}