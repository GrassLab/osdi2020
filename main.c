#include "uart.h"

int command_help ();
int command_hello ();

int main()
{
    // set up serial console
    uart_init();
    
    // say hello
    uart_puts("Hello World!\n");
    
    // echo everything back
    while(1)
    {
        uart_send(uart_getc());
    }

    return 0;
}

int command_helo ()
{
    return 0;
}

int command_hello ()
{
    return 0;
}