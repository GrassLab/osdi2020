#include "lib/task.h"

#include "kernel/peripherals/time.h"
#include "kernel/peripherals/uart.h"

#include "lib/type.h"

#include "mem.h"
#include "schedule.h"
#include "task.h"
#include "task_queue.h"

thread_info_t * IDLE = NULL;
thread_info_t * THREAD_POOL[NUM_THREADS];

/* only can be used in this file */
thread_info_t * create_thread_info ( void ( *func ) ( ) );

void create_idle_task ( )
{
    pcb_t * idle_pcb = allocate_pcb ( );

    IDLE                = idle_pcb->thread_info;
    IDLE->state         = RUNNING;
    IDLE->func          = idle;
    IDLE->priority      = 1;
    IDLE->const_counter = 0;
    IDLE->counter       = 0;

    IDLE->parent = NULL;
    IDLE->child  = NULL;

    IDLE->kernel_sp = idle_pcb->kernel_stack_ptr;
    IDLE->user_sp   = idle_pcb->user_stack_ptr;

    ( IDLE->cpu_context ).x[19]        = (uint64_t *) idle;
    ( IDLE->cpu_context ).lr           = (uint64_t *) default_task_start;
    ( IDLE->cpu_context ).user_sp      = (uint64_t *) idle_pcb->user_stack_ptr;
    ( IDLE->cpu_context ).kernel_sp    = (uint64_t *) idle_pcb->kernel_stack_ptr;
    ( IDLE->cpu_context ).user_mode_pc = (uint64_t *) default_task_start;
}

thread_info_t * create_thread_info ( void ( *func ) ( ) )
{
    /* allocate TCB in kernel space */
    pcb_t * new_task            = (pcb_t *) allocate_pcb ( );
    thread_info_t * thread_info = new_task->thread_info;

    /* init a task */
    thread_info->state         = RUNNING;
    thread_info->func          = func;
    thread_info->priority      = 1; /* all task has the same priority */
    thread_info->const_counter = 2;
    thread_info->counter       = 2;

    thread_info->parent = NULL;
    thread_info->child  = NULL;

    thread_info->kernel_sp = new_task->kernel_stack_ptr;
    thread_info->user_sp   = new_task->user_stack_ptr;

    ( thread_info->cpu_context ).x[19]        = (uint64_t *) func;
    ( thread_info->cpu_context ).lr           = (uint64_t *) default_task_start;
    ( thread_info->cpu_context ).user_sp      = (uint64_t *) ( new_task->user_stack_ptr );
    ( thread_info->cpu_context ).kernel_sp    = (uint64_t *) ( new_task->kernel_stack_ptr );
    ( thread_info->cpu_context ).user_mode_pc = (uint64_t *) default_task_start;

    sys_printk ( "[TASK]\t\tCreate Task with task_id: %d\n", thread_info->task_id );

    return thread_info;
}

int task_create ( void ( *func ) ( ) )
{
    thread_info_t * new_task = create_thread_info ( func );

    if ( new_task == NULL )
        return -1;

    /* save it into the pool */
    THREAD_POOL[new_task->task_id] = new_task;

    /* put the task into the runqueue */
    task_enqueue ( new_task );

    return new_task->task_id;
}

void set_thread_const_couner ( int pid, int v )
{
    THREAD_POOL[pid]->const_counter = v;
    THREAD_POOL[pid]->counter       = THREAD_POOL[pid]->counter > v ? v : THREAD_POOL[pid]->counter;
}

thread_info_t * get_thread_info ( int pid )
{
    return THREAD_POOL[pid];
}

thread_info_t * sys_duplicate_task ( thread_info_t * current_task )
{
    thread_info_t * new_task = create_thread_info ( current_task->func );

    if ( new_task == NULL )
        return NULL;

    new_task->priority = current_task->priority;
    new_task->counter  = current_task->counter;

    new_task->parent    = current_task;
    current_task->child = new_task;

    /* copy cpu_context */
    int i;
    /* set the return value as 0 for the child task */
    ( new_task->cpu_context ).x[0] = 0;
    /* copy register, start from 1 */
    for ( i = 1; i < 29; i++ )
        ( new_task->cpu_context ).x[i] = ( new_task->cpu_context ).x[i];
    ( new_task->cpu_context ).fp           = ( new_task->user_sp ) - ( current_task->user_sp - ( ( current_task->cpu_context ).user_sp ) ); /* count the offset */
    ( new_task->cpu_context ).lr           = ( current_task->cpu_context ).lr;                                                              /* return address remain the same */
    ( new_task->cpu_context ).user_sp      = ( new_task->user_sp ) - ( current_task->user_sp - ( ( current_task->cpu_context ).user_sp ) ); /* count the offset */
    ( new_task->cpu_context ).kernel_sp    = new_task->kernel_sp;
    ( new_task->cpu_context ).user_mode_pc = ( current_task->cpu_context ).user_mode_pc; /* program counter remain the same */
    /* copy memory */
    int size = ( uint64_t ) ( (char *) current_task->user_sp - (char *) ( ( current_task->cpu_context ).user_sp ) );
    for ( i = 0; i < size; i++ )
        ( (char *) ( ( new_task->cpu_context ).user_sp ) )[i] = ( (char *) ( ( current_task->cpu_context ).user_sp ) )[i];

    /* save it into the pool */
    THREAD_POOL[new_task->task_id] = new_task;

    /* put the task into the runqueue */
    task_enqueue ( new_task );

    return new_task;
}

void clear_zombie ( )
{
    int i;
    for ( i = 0; i < NUM_THREADS; i++ )
    {
        if ( THREAD_POOL[i]->state == ZOMBIE )
        {
            sys_printk ( "Release Zombie: %d\n", THREAD_POOL[i]->task_id );
            THREAD_POOL[i]->state = DEAD;
            release_pcb ( (pcb_t *) THREAD_POOL[i] );
        }
    }
    sys_wait_msec ( 500000 );
}