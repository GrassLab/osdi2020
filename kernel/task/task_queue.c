#include "task_queue.h"

#include "kernel/peripherals/uart.h"

#include "lib/type.h"

#include "mem.h"
#include "task.h"

thread_info_t * QUEUE[NUM_THREADS - 1]; /* there is one are used for idle task */
int QUEUE_HEAD                = 0;
int QUEUE_TAIL                = 0;
const int QUEUE_ACCOMMPDATION = NUM_THREADS - 1;
int IS_FULL                   = 0;
extern thread_info_t * IDLE;

task_queue_state_t task_enqueue ( thread_info_t * task )
{
    /* queue is full */
    if ( IS_FULL )
        return QUEUE_FULL;

    if ( QUEUE_HEAD <= QUEUE_TAIL && !IS_FULL )
    {
        QUEUE[QUEUE_TAIL % QUEUE_ACCOMMPDATION] = task;

        QUEUE_TAIL++;

        if ( QUEUE_TAIL - QUEUE_HEAD + 1 == QUEUE_ACCOMMPDATION )
            IS_FULL = 1;

        // sys_printk ( "[TASK QUEUE]\tTask Enqueue: %d\n", task->task_id );

        return SUCCESS;
    }

    return UNKNOWN;
}

thread_info_t * task_dequeue ( )
{
    thread_info_t * temp;

    /* queue is empty */
    if ( QUEUE_HEAD == QUEUE_TAIL && !IS_FULL )
        return IDLE;

    temp = QUEUE[QUEUE_HEAD];
    QUEUE_HEAD++;
    IS_FULL = 0;

    // sys_printk ( "[TASK QUEUE]\tTask Dequeue: %d\n", temp->task_id );

    return temp;
}