#include "uart.h"
#include "type.h"
#include "mailbox.h"
#include "string.h"
#include "shell.h"

int main()
{
    // set up serial console
    uart_init();
    
    // say hello
    uart_puts("Hello World!\n");

    // start shell
    shell_start();
    
    return 0;
}