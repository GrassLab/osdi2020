#ifndef __SYS_TASK_QUEUE_H
#define __SYS_TASK_QUEUE_H

#include "task.h"

typedef enum {

    SUCCESS = 0,
    
    QUEUE_FULL = -1,
    QUEUE_EMPTY = -2,
    UNKNOWN = -3,

} task_queue_state_t;

task_queue_state_t task_enqueue ( thread_info_t * task );
thread_info_t * task_dequeue ( );

#endif