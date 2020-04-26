#include "lib/type.h"
#include "kernel/peripherals/uart.h"

#include "task_queue.h"

task_t * QUEUE[64];
int QUEUE_HEAD = 0;
int QUEUE_TAIL = 0;
const int QUEUE_ACCOMMPDATION = 64;
int IS_FULL = 0;

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

        uart_printf("[TASK QUEUE] Task Enqueue: %x\n", task -> task_id );
        
        return SUCCESS;
    }

    return UNKNOWN;
}

task_t * task_dequeue ( )
{
    task_t * temp;
    
    if ( QUEUE_HEAD == QUEUE_TAIL && !IS_FULL )
        return NULL;

    temp = QUEUE[QUEUE_HEAD];
    QUEUE_HEAD ++;
    IS_FULL = 0;

    uart_printf("[TASK QUEUE] Task Dequeue: %x\n", temp -> task_id );

    return temp;
}