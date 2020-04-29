#include "kernel/exception/exception.h"
#include "kernel/peripherals/uart.h"

#include "lib/time.h"

void idle ( )
{
    while ( 1 )
    {
        uart_printf("I am idle.\n");
        
        LAUNCH_SYS_CALL ( SYS_CALL_SCHEDULE );

        wait_msec(500000);
    }
}