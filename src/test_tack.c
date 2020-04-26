#include "kernel/peripherals/time.h"
#include "kernel/peripherals/uart.h"
#include "kernel/task/task.h"

extern task_t * TASK_POOL[];

// this would be placed at task[1]
void task_1 ()
{
    while ( 1 )
    {
        uart_printf("TEST TASK 1\n");
        wait_msec(500000);
        context_switch ( TASK_POOL[2] );
    }
    
}

// this wouble be placed at task[2]
void task_2 ()
{
    while ( 1 )
    {
        uart_printf("TEST TASK 2\n");
        wait_msec(500000);
        context_switch ( TASK_POOL[1] );
    }
}

void task_switch_test ()
{
    context_switch ( TASK_POOL[1] );
}