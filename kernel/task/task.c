#include "kernel/exception/exception.h"
#include "kernel/exception/timer.h"
#include "kernel/peripherals/time.h"
#include "kernel/peripherals/uart.h"
#include "lib/type.h"

#include "mem.h"
#include "task.h"
#include "task_queue.h"

task_t INIT = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 0, 1, 0, 0, 0, 0};
task_t * IDLE = NULL;
task_t * CURRENT_TASK = &INIT;

task_t * TASK_POOL[NUM_THREADS];

/* 64 bit to maintain which one is in used */
uint64_t TASK_POOL_USAGE = 0;

void idle ( )
{
    while ( 1 )
    {
        uart_printf("I am idle.\n");
        schedule ( );
        wait_msec(500000);
    }
}

int privilege_task_create ( void(*func)() )
{
    /* create IDLE task at first */
    if ( IDLE == NULL )
    {
        IDLE = ( task_t * ) ( LOW_MEM );
        IDLE -> state = RUNNING;
        IDLE -> func = idle;
        IDLE -> stack = ((char *)IDLE) + THREAD_SIZE;
        IDLE -> priority = 1;
        IDLE -> counter = 0;

        (IDLE -> cpu_context).x19 = (unsigned long)idle;
        (IDLE -> cpu_context).lr  = (unsigned long)default_task_start;
        (IDLE -> cpu_context).sp  = (unsigned long)(((char *)IDLE) + THREAD_SIZE);

        /* update current task to idle */
        CURRENT_TASK = IDLE;

        /* reserve one for idle task*/
        TASK_POOL_USAGE = 1;
    }

    /* find the one that can be used */
    int task_id = find_usable_in_pool ();

    if ( task_id == -1 )
        return -1;
    
    TASK_POOL_USAGE |= ( 0b1 << task_id );

    /* allocate TCB in kernel space */
    task_t * new_task = ( task_t * ) ( ( LOW_MEM ) +  ( THREAD_SIZE * task_id ) );
 
    /* init a task */
    new_task -> task_id = task_id;
    new_task -> state = RUNNING;
    new_task -> func = func;
    new_task -> stack = ((char *)new_task) + THREAD_SIZE;
    new_task -> priority = 1;   /* all task has the same priority */
    new_task -> counter = 1;

    (new_task -> cpu_context).x19 = (unsigned long)func;
    (new_task -> cpu_context).lr = (unsigned long)default_task_start;
    (new_task -> cpu_context).sp = (unsigned long)(((char *)new_task) + THREAD_SIZE);

    /* save it into the pool */
    TASK_POOL[task_id] = new_task;

    /* put the task into the runqueue */
    task_enqueue ( new_task );

    uart_printf( "[TASK] Create Task with task_id: %x\n", task_id );

    return task_id;
}

int find_usable_in_pool ()
{
    int i;

    for ( i = 0; i < 64; i++ )
    {
        if ( (TASK_POOL_USAGE & (0b1 << i)) == 0 )
        {
            return i;
        }
    }

    return -1;
}

void context_switch ( task_t * next )
{
    /* if the next one is the same as the current, do nothing */
    if ( CURRENT_TASK == next )
    {
        /* enable irq before leaving */
        LAUNCH_SYS_CALL ( SYS_CALL_IRQ_EL1_ENABLE );
        return ;
    }
        
    task_t * prev = CURRENT_TASK;

    CURRENT_TASK = next;

    LAUNCH_SYS_CALL ( SYS_CALL_IRQ_EL1_ENABLE );    

    switch_to ( prev, next );
}

void schedule ( )
{
    /* current task still have the time to do something it want */
    if ( CURRENT_TASK -> counter > 0)
    {        
        ( CURRENT_TASK -> counter ) --;

        /* enable ireq again */
        LAUNCH_SYS_CALL ( SYS_CALL_IRQ_EL1_ENABLE );    
        return;
    }

    /* current task spend all its time, switch to the next one */
    task_t * next = task_dequeue ( );

    /* switch to the next one */
    context_switch ( next );
}