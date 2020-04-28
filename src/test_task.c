#include "kernel/exception/exception.h"
#include "kernel/peripherals/time.h"
#include "kernel/peripherals/uart.h"
#include "kernel/task/task.h"

void test_task ( )
{
    int i = 6;

    thread_info_t * task = get_current_task ( );

    while ( i-- )
    {   
        uart_printf("This is TASK %d\n", task -> task_id);
        wait_msec(1000000);

        schedule ( );
    }

    uart_printf("This is TASK %d\n", task -> task_id);

    /* update self state before leaving */
    task -> state = DEAD;

    /* call schedule before leaving */ 
    schedule ( );
}

void task_schedule_test ( )
{
    int i;
    for ( i = 0; i < 5; i++ )
    {
        privilege_task_create ( test_task );
    }
    
    /* enable irq for scheduling */ 
    LAUNCH_SYS_CALL ( SYS_CALL_IRQ_EL1_ENABLE );
    LAUNCH_SYS_CALL ( SYS_CALL_CORE_TIMER_ENABLE );    

    /* launch init task */
    launch_init ( );
}