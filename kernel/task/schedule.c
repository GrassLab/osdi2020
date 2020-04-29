#include "kernel/exception/exception.h"

#include "schedule.h"
#include "task_queue.h"
#include "task.h"

int RESCHED_FLAG = 0;

void context_switch ( thread_info_t * next )
{
    thread_info_t * current_task = get_current_task_el0 ( );
    
    /* if the next one is the same as the current, do nothing */
    if ( current_task == next )
    {
        /* enable irq before leaving */
        // LAUNCH_SYS_CALL ( SYS_CALL_IRQ_EL1_ENABLE );
        return ;
    }
    
    /* enable irq before leaving */
    // LAUNCH_SYS_CALL ( SYS_CALL_IRQ_EL1_ENABLE );    

    set_next_task_el0 ( next );

    // switch_to ( current_task, next );
}

void schedule ( )
{
    thread_info_t * current_task = get_current_task_el0 ( );

    /* check if need to resched */
    if ( !RESCHED_FLAG && current_task -> state == RUNNING )
        return;    
    
    /* while scheduling, disable irq */
    // LAUNCH_SYS_CALL ( SYS_CALL_IRQ_EL1_DISABLE );    

    /* reset resched flag */
    RESCHED_FLAG = 0;

    /* enqueue current task to the run queue, if it needed */
    if ( current_task -> state == RUNNING && current_task != IDLE )
    {
        current_task -> counter = 1;
        task_enqueue ( current_task );
    }        

    /* current task spend all its time, switch to the next one */
    thread_info_t * next = task_dequeue ( );

    /* switch to the next one */
    context_switch ( next );
}

void do_exec ( void(*func)() )
{
    int pid = task_create ( func );
    thread_info_t * thread_info = get_thread_info ( pid );
    context_switch ( thread_info );    
}