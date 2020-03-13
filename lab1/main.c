#include "uart.h"

void main()
{
    // set up serial console
    uart_init();
    
    // say hello
    uart_puts("Hello World!\n");
    
    // echo everything back
    while(1) {
	char g = uart_getc();
	if(g == '\n')
	    uart_send('\r');
	uart_send(g);
    }
}
