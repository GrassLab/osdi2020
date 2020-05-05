#include "stdint.h"

#ifndef __SCHED_H__
#define __SCHED_H__

#define MAX_TASKS 64

extern struct task_struct *current;
extern struct task_struct *task_pool[MAX_TASKS];
extern int running_tasks;

struct cpu_context {
    // ARM calling convention
    // x0 - x18 can be overwritten by the called function
    uint64_t x19;
    uint64_t x20;
    uint64_t x21;
    uint64_t x22;
    uint64_t x23;
    uint64_t x24;
    uint64_t x25;
    uint64_t x26;
    uint64_t x27;
    uint64_t x28;
    uint64_t fp;  // x29
    uint64_t lr;  // x30
    uint64_t sp;
};

enum task_state {
    RUNNING,
};

struct task_struct {
    uint64_t id;
    enum task_state state;
    struct cpu_context cpu_context;
};

#endif
