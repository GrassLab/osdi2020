#include "lib/type.h"

#include "mem.h"
#include "task.h"
#include "task_queue.h"

task_t * task_pool[NUM_THREADS];
//char kstack_pool[64][4096];

/* 64 bit to maintain which one can use */
uint64_t task_pool_usage = 0;

int privilege_task_create ( void(*func)() )
{
    /* find the one that can be used */
    int task_id = find_usable_in_pool ();

    if ( task_id == -1 )
        return -1;
    
    task_pool_usage |= ( 0b1 << task_id );

    /* allocate TCB in kernel space */
    task_t * new_task = ( task_t * ) ( ( LOW_MEM ) +  ( THREAD_SIZE * task_id) );

    /* init a task */
    new_task -> task_id = task_id;
    new_task -> state = RUNNING;
    new_task -> func = func;

    (new_task -> cpu_context).x19 = (unsigned long)func;
    (new_task -> cpu_context).sp = (unsigned long)(new_task + THREAD_SIZE);

    return task_id;
}

int find_usable_in_pool ()
{
    int i;

    for ( i = 0; i < 64; i++ )
    {
        if ( (task_pool_usage & (0b1 << i)) == 0 )
        {
            return i;
        }
    }

    return -1;
}
