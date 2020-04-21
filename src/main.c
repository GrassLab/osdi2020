#include <uart.h>
#include <shell.h>
#include <irq.h>

void main()
{
    uart0_init();   // set up serial console, UART0
	irq_init();     // enable uart_int 

	shell_start();

}
