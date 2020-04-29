#include "kernel/peripherals/uart.h"
#include "lib/task.h"
#include "lib/time.h"

#include "shell.h"

int main ( )
{
    // set up serial console
    uart_init();

    // say hello
    uart_printf("Hello World\n");

    while ( 1 )
    {
        uart_printf("Here is main.\n");
        wait_msec ( 1000000 );
    }
    
    return 0;
}