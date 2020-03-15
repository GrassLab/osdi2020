#include "../include/uart.h"
void main()
{
    // set up serial console
    uart_init();
    
    // say hello
    uart_puts("This is Lab1\n");
    
    // echo everything back
    for(;;)
    {
        char double_output;
        double_output = uart_getc();
        uart_send(double_output);
        uart_send(double_output);
    }
}
