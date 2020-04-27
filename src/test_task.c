#include "kernel/exception/exception.h"
#include "kernel/exception/timer.h"
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
    }
    
}

// this wouble be placed at task[2]
void task_2 ()
{
    while ( 1 )
    {
        uart_printf("TEST TASK 2\n");
        wait_msec(500000);
    }
}

void task_schedule_test ()
{
    privilege_task_create ( task_1 );
    privilege_task_create ( task_2 ); 

    LAUNCH_SYS_CALL ( SYS_CALL_IRQ_EL1_ENABLE );
    LAUNCH_SYS_CALL ( SYS_CALL_CORE_TIMER_ENABLE );    

    launch_init ( );
}