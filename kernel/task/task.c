#include "lib/type.h"

#include "mem.h"
#include "task.h"
#include "task_queue.h"

#include "kernel/peripherals/uart.h"

static task_t INIT = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 1, 0, 0, 0};
task_t * CURRENT_TASK = &INIT;

task_t * TASK_POOL[NUM_THREADS] = { &INIT, };

/* 64 bit to maintain which one can use */
uint64_t TASK_POOL_USAGE = 1;

int privilege_task_create ( void(*func)() )
{
    /* find the one that can be used */
    int task_id = find_usable_in_pool ();

    if ( task_id == -1 )
        return -1;
    
    TASK_POOL_USAGE |= ( 0b1 << task_id );

    /* allocate TCB in kernel space */
    task_t * new_task = ( task_t * ) ( ( LOW_MEM ) +  ( THREAD_SIZE * task_id) );
 
    /* init a task */
    new_task -> task_id = task_id;
    new_task -> state = RUNNING;
    new_task -> func = func;
    new_task -> stack = ((char *)new_task) + THREAD_SIZE;

    (new_task -> cpu_context).x19 = (unsigned long)func;
    (new_task -> cpu_context).lr = (unsigned long)default_task_start;
    (new_task -> cpu_context).sp = (unsigned long)(((char *)new_task) + THREAD_SIZE);

    /* save it into the pool */
    TASK_POOL[task_id] = new_task;

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
        return ;

    task_t * prev = CURRENT_TASK;

    CURRENT_TASK = next;
    
    switch_to ( prev, next );
}
