#include "kernel/peripherals/uart.h"

#include "lib/task.h"
#include "lib/time.h"

void test_task ( )
{
    int i = 5;
    int pid = get_pid ( );

    while ( i-- )
    {   
        uart_printf("I am task %d\n", pid);
        wait_msec(3000000);
    }
}