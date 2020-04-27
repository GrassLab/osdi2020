#include "kernel/exception/exception.h"
#include "kernel/peripherals/time.h"
#include "kernel/peripherals/uart.h"
#include "kernel/task/task.h"

void test_task ()
{
    while ( 1 )
    {
        task_t * task = get_current_task ( );
        
        uart_printf("This is TASK %d\n", task -> task_id);
        wait_msec(500000);
    }
    
}

void task_schedule_test ()
{
    int i;
    for ( i = 0; i < 10; i++ )
    {
        privilege_task_create ( test_task );
    }
    
    /* enable irq for scheduling */ 
    LAUNCH_SYS_CALL ( SYS_CALL_IRQ_EL1_ENABLE );
    LAUNCH_SYS_CALL ( SYS_CALL_CORE_TIMER_ENABLE );    

    /* launch init task */
    launch_init ( );
}