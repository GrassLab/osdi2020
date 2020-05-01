#include "kernel/peripherals/uart.h"
#include "lib/type.h"
#include "lib/task.h"

#include "mem.h"
#include "schedule.h"
#include "task_queue.h"
#include "task.h"

thread_info_t * IDLE = NULL;
thread_info_t * TASK_POOL[NUM_THREADS];

uint64_t TASK_POOL_USAGE = 0;       /* 64 bit to maintain which one is in used */

void create_idle_task ( )
{
    pcb_t * idle_pcb = allocate_pcb ( 0 );

    IDLE = idle_pcb -> thread_info;
    IDLE -> state = RUNNING;
    IDLE -> func = idle;
    IDLE -> priority = 1;
    IDLE -> counter = 0;

    IDLE -> parent = NULL;
    IDLE -> child = NULL;

    (IDLE -> cpu_context).x[19] =   (unsigned long)idle;
    (IDLE -> cpu_context).lr =      (unsigned long)default_task_start;
    (IDLE -> cpu_context).user_sp =         (unsigned long)idle_pcb -> user_stack_ptr;
    (IDLE -> cpu_context).kernel_sp =       (unsigned long)idle_pcb -> kernel_stack_ptr;
    (IDLE -> cpu_context).user_mode_pc =    (unsigned long)default_task_start;

    /* reserve one for idle task*/
    TASK_POOL_USAGE = 1;
}

int task_create ( void(*func)() )
{
    /* find the one that can be used */
    int task_id = find_usable_in_pool ();

    if ( task_id == -1 )
        return -1;
    
    TASK_POOL_USAGE |= ( 0b1 << task_id );

    /* allocate TCB in kernel space */
    pcb_t * new_task = ( pcb_t * ) allocate_pcb ( task_id );
    thread_info_t * thread_info = new_task -> thread_info;
 
    /* init a task */
    thread_info -> task_id = task_id;
    thread_info -> state = RUNNING;
    thread_info -> func = func;
    thread_info -> priority = 1;   /* all task has the same priority */
    thread_info -> counter = 2;

    IDLE -> parent = NULL;
    IDLE -> child = NULL;

    (thread_info -> cpu_context).x[19] =    ( unsigned long)func;
    (thread_info -> cpu_context).lr =       (unsigned long)default_task_start;
    (thread_info -> cpu_context).user_sp =  (unsigned long)(new_task -> user_stack_ptr);
    (thread_info -> cpu_context).kernel_sp =    (unsigned long)(new_task -> kernel_stack_ptr);
    (thread_info -> cpu_context).user_mode_pc = (unsigned long)default_task_start;

    /* save it into the pool */
    TASK_POOL[task_id] = thread_info;

    sys_printk( "[TASK]\t\tCreate Task with task_id: %d\n", task_id );

    /* put the task into the runqueue */
    task_enqueue ( thread_info );

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

thread_info_t * get_thread_info ( int pid )
{
    return TASK_POOL[pid];
}