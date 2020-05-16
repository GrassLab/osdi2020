#include "stdint.h"

#ifndef __SCHEDULE_H__
#define __SCHEDULE_H__

#define TASK_POOL_SIZE  64
#define KSTACK_SIZE     4096

extern struct task_t *current;
extern struct task_t *task_pool[];
extern char *kstack_pool[];

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
    EXIT,
};

struct task_t {
    uint64_t id;
    enum task_state state;
    struct cpu_context cpu_context;
};

void schedule_init();
void privilege_task_create(void (*func)());

#endif
