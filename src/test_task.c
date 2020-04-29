#include "kernel/peripherals/uart.h"

#include "lib/time.h"

void test_task ( )
{
    unsigned int i = 5;
    while ( i-- )
    {   
        uart_printf("I am test TASK\n");
        wait_msec(1000000);
    }
}