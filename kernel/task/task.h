#ifndef TASK_H
#define TASK_H

#include "lib/type.h"

typedef enum {
    RUNNING = 1,
} task_state_t;

typedef struct {

    unsigned long x19;
    unsigned long x20;
    unsigned long x21;
    unsigned long x22;
    unsigned long x23;
    unsigned long x24;
    unsigned long x25;
    unsigned long x26;
    unsigned long x27;
    unsigned long x28;
    unsigned long fp;   // x29
    unsigned long lr;   // x30
    unsigned long sp;

} context_t;

typedef struct {
    
    context_t cpu_context;
    task_state_t state;
    
    uint64_t task_id;
    void (*func)();
    char * stack;

} task_t;

int privilege_task_create ( void(*func)() );
int find_usable_in_pool ();
void context_switch ( task_t * next );

/* defined in task.S */
extern void switch_to ( task_t *, task_t * );
extern task_t* get_current_task ();
extern void default_task_start();

#endif