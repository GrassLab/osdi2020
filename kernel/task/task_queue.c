#include "task_queue.h"

static task_t * QUEUE[64];
static int QUEUE_HEAD;
static int QUEUE_TAIL;
static const int QUEUE_ACCOMMPDATION = 64;
static int IS_FULL = 0;

void task_queue_init ()
{
    QUEUE_HEAD = 0;
    QUEUE_TAIL = 0;
}

TASK_QUEUE_STATE task_enqueue ( task_t * task )
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
        
        return SUCCESS;
    }

    return UNKNOWN;
}

task_t * task_dequeue ( )
{
    task_t * temp;
    if ( QUEUE_HEAD == QUEUE_TAIL && !IS_FULL )
        return (task_t *)QUEUE_EMPTY;

    temp = QUEUE[QUEUE_HEAD];
    QUEUE_HEAD ++;
    IS_FULL = 0;

    return temp;
}

void schedule ( )
{
    
}