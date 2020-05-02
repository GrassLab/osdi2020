#ifndef _SCHED_H
#define _SCHED_H

#define MAX_NUM_TASKS 64

/* static allocation NUM_TASKS */
/* Error will happend if used_task_id > MAX_NUM_TASKS */ 

typedef long tid_t;
enum state_t {TASK_RUNNING};

/* 0 is for init_task */
static tid_t used_task_id = 0;

struct cpu_context {
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
    unsigned long fp;  // x29 
    unsigned long pc;  // x30
    unsigned long sp; 
};

struct task_struct {
    struct cpu_context cpu_context;
    tid_t task_id;
    enum state_t state;
    long counter;
    long priority;
    long preempt_count;
    // unsigned long stack;
    // unsigned long flags;
    // unsigned long kill_flag;
};
static struct task_struct task_pool[MAX_NUM_TASKS];

#define INIT_TASK \
{ {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, \
   0, TASK_RUNNING, 0, 1, 0}

tid_t acquire_unused_task_id();
#endif
