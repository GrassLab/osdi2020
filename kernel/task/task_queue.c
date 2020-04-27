#include "lib/type.h"
#include "kernel/peripherals/uart.h"

#include "mem.h"
#include "task_queue.h"
#include "task.h"

task_t * QUEUE[THREAD_SIZE - 1];    /* there is one are used for idle task */
int QUEUE_HEAD = 0;
int QUEUE_TAIL = 0;
const int QUEUE_ACCOMMPDATION = THREAD_SIZE - 1;
int IS_FULL = 0;
extern task_t * IDLE;

task_queue_state_t task_enqueue ( task_t * task )
{
    /* queue is full */
    if ( IS_FULL )
        return QUEUE_FULL;

    if ( QUEUE_HEAD <= QUEUE_TAIL && !IS_FULL )
    {
        QUEUE[QUEUE_TAIL % QUEUE_ACCOMMPDATION] = task;
        
        QUEUE_TAIL++;

        if ( QUEUE_TAIL - QUEUE_HEAD + 1  == QUEUE_ACCOMMPDATION )
            IS_FULL = 1;

        uart_printf("[TASK QUEUE]\tTask Enqueue: %d\n", task -> task_id );
        
        return SUCCESS;
    }

    return UNKNOWN;
}

task_t * task_dequeue ( )
{
    task_t * temp;
    
    /* queue is empty */
    if ( QUEUE_HEAD == QUEUE_TAIL && !IS_FULL )
        return IDLE;

    temp = QUEUE[QUEUE_HEAD];
    QUEUE_HEAD ++;
    IS_FULL = 0;

    uart_printf("[TASK QUEUE]\tTask Dequeue: %d\n", temp -> task_id );

    return temp;
}