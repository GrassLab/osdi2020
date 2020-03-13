#include "../Include/uart.h"
void main()
{
    // set up serial console
    uart_init();
    
    // say hello
    uart_puts("This is Lab1\n");
    
    // echo everything back
    for(;;)
    {
        uart_send(uart_getc());
    }
}
