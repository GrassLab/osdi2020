#include "uart.h"
#include "shell.h"

void main()
{
    // set up serial console, UART0
    uart0_init(0);

	shell_start();

}
