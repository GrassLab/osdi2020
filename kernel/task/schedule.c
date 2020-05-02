#include "schedule.h"

#include "kernel/exception/exception.h"
#include "kernel/peripherals/uart.h"

#include "task.h"
#include "task_queue.h"

void context_switch ( thread_info_t * next )
{
    thread_info_t * current_task = get_current_task_el0 ( );

    /* if the next one is the same as the current, do nothing */
    if ( current_task == next )
        return;

    set_next_task_el0 ( next );

    ( next->counter )--;
}

void sys_do_schedule ( )
{
    thread_info_t * current_task = get_current_task_el0 ( );

    /* check if need to resched */
    /* current task still have time to do something */
    /* CURRENT TASK still doing something */
    /* this is not called by IDLE */
    if ( current_task->counter > 0 && current_task->state == RUNNING && current_task != IDLE )
    {
        ( current_task->counter )--;
        return;
    }

    /* enqueue current task to the run queue, if it needed */
    if ( current_task->state == RUNNING && current_task != IDLE )
    {
        current_task->counter = 2;
        task_enqueue ( current_task );
    }

    /* current task spend all its time, switch to the next one */
    thread_info_t * next;
    while ( 1 )
    {
        next = task_dequeue ( );

        if ( next->state != RUNNING )
        {
            task_enqueue ( next );
        }
        else if ( next->state == RUNNING && next->counter < 0 )
        {
            next->counter = 2;
            task_enqueue ( next );
        }
        else
        {
            break;
        }
    }
    /* switch to the next one */
    context_switch ( next );
}

void sys_do_exec ( void ( *func ) ( ) )
{
    /* get cuurent task */
    thread_info_t * current_thread = get_current_task_el0 ( );

    /* create thread for the target func */
    int c_pid = task_create ( func );

    /* change the state to the DEAD */
    current_thread->state = DEAD;
    /* enqueu current task, it will be used lated */
    // task_enqueue ( current_thread );

    current_thread->child         = get_thread_info ( c_pid );
    current_thread->child->parent = current_thread;

    sys_do_schedule ( );
}

void sys_do_exit ( thread_info_t * thread )
{
    /* wake parent up */
    if ( thread->parent && thread->parent->state == WAITING_CHILD )
    {
        thread->parent->state = RUNNING;
    }

    if ( thread->parent )
        thread->parent->child = NULL;

    /* mark this thread as a dead one */
    thread->state = DEAD;

    sys_do_schedule ( );
}