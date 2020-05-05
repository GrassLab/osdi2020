#include "stdint.h"

#ifndef __SCHEDULE_H__
#define __SCHEDULE_H__

#define TASK_POOL_SIZE 64

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

struct task_struct {
    uint64_t id;
    enum task_state state;
    struct cpu_context cpu_context;
    uint64_t flag;
    /*
     * bit 0: reschedule
     * bit 1: preemptable
     */
    uint64_t priority;
    uint64_t counter;
};

#define INIT_TASK       {0, RUNNING, {0,0,0,0,0,0,0,0,0,0,0,0,0}}
#define INIT_FLAG       0b10
#define INIT_PRIORITY   1

#define RESHEDULE_BIT               0
#define PREEMPTABLE_BIT             1

#define RESHEDULE(flag)             (flag & (1 << RESHEDULE_BIT)) >> RESHEDULE_BIT
#define PREEMPTABLE(flag)           (flag & (1 << PREEMPTABLE_BIT)) >> PREEMPTABLE_BIT

#endif

extern struct task_struct *current;

void schedule_init();
void privilege_task_create(void(*func)());
void context_switch(struct task_struct *next);
void schedule();
extern void switch_to(struct cpu_context* prev, struct cpu_context* next);
