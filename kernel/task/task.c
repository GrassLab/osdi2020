#include "kernel/peripherals/time.h"
#include "kernel/peripherals/uart.h"
#include "lib/type.h"

#include "mem.h"
#include "schedule.h"
#include "task_queue.h"
#include "task.h"

task_t * IDLE = NULL;
task_t * TASK_POOL[NUM_THREADS];

uint64_t TASK_POOL_USAGE = 0;       /* 64 bit to maintain which one is in used */

void idle ( )
{
    while ( 1 )
    {
        uart_printf("I am idle.\n");
        RESCHED_FLAG = 1;
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
    new_task -> counter = 2;

    (new_task -> cpu_context).x19 = (unsigned long)func;
    (new_task -> cpu_context).lr = (unsigned long)default_task_start;
    (new_task -> cpu_context).sp = (unsigned long)(((char *)new_task) + THREAD_SIZE);

    /* save it into the pool */
    TASK_POOL[task_id] = new_task;

    uart_printf( "[TASK]\t\tCreate Task with task_id: %d\n", task_id );

    /* put the task into the runqueue */
    task_enqueue ( new_task );

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