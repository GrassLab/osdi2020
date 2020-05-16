#include "stdint.h"

#ifndef __SCHEDULE_H__
#define __SCHEDULE_H__

#define TASK_POOL_SIZE 64
#define KSTACK_SIZE 4096

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
    int priority;
    struct cpu_context cpu_context;
};

/* Variables init in schedule.c */
extern struct task_t task_pool[TASK_POOL_SIZE];
extern char kstack_pool[TASK_POOL_SIZE][KSTACK_SIZE];

/* Function in schedule.S */
extern struct task_t* get_current_task();
extern void update_current_task(struct task_t *task);
extern void switch_to(struct cpu_context* prev, struct cpu_context* next);

/* Function in schedule.c */
void schedule_init();
void privilege_task_create(void (*func)(), int priority);
void context_switch(struct task_t* next);
void schedule();

#endif
