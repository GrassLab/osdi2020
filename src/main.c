#include "kernel/peripherals/uart.h"
#include "lib/task.h"
#include "lib/time.h"

#include "shell.h"

int main ( )
{
    while ( 1 )
    {
        uart_printf("Here is main.\n");
        wait_msec ( 1000000 );
    }
    
    return 0;
}