#ifndef TASK_QUEUE_H
#define TASK_QUEUE_H

#include "task.h"

typedef enum {

    SUCCESS = 0,
    
    QUEUE_FULL = -1,
    QUEUE_EMPTY = -2,
    UNKNOWN = -3,

} TASK_QUEUE_STATE;

void task_queue_init ( );
TASK_QUEUE_STATE task_enqueue ( task_t * task );
task_t * task_dequeue ( );
void schedule ( );

#endif