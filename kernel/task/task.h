#ifndef __TASK_H
#define __TASK_H

#include "lib/type.h"

typedef enum {
    RUNNING = 1,
    DEAD = 2,
    IDLE_STATE = 3,
} task_state_t;

typedef struct {

    unsigned long x[29];    // x0~x28 
    unsigned long fp;       // x29
    unsigned long lr;       // x30
    unsigned long user_sp;
    unsigned long kernel_sp;
    unsigned long user_mode_pc; /* to save the exec location, while trapped into kernel mode  */

} context_t;

typedef struct {
    
    context_t cpu_context;    
    
    uint64_t task_id;
    task_state_t state;
    void (*func)();    

    uint64_t counter;
    uint64_t priority;  /* at this time, all task has the same priority */

} thread_info_t;

/* global variable for other file to use */
extern thread_info_t * IDLE;

void create_idle_task ( );
int task_create ( void(*func)() );
int find_usable_in_pool ( );
thread_info_t * get_thread_info ( int pid );

#endif