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
    ZOMBIE,
    EXIT,
};

struct task_struct {
    uint64_t id;
    enum task_state state;
    struct cpu_context cpu_context;
    uint64_t priority;
    uint64_t counter;
    int exit_status;
    // stack pointer
    char *kstack;  // decide after privilege_task_create
    char *ustack;  // decide after exec / fork
    /*
     * bit 0: reschedule
     * bit 1: preemptable
     * bit 2: kill
     */
    uint64_t flag;
};

#define INIT_PRIORITY       5

#define INIT_FLAG           0b010
#define RESCHEDULE_BIT      0
#define PREEMPTABLE_BIT     1
#define KILL_BIT            2

#define HAS(flag, bit) (flag & (1 << bit))
#define SET(flag, bit) flag |= (1 << bit)
#define CLR(flag, bit)        \
    uint64_t mask = 1 << bit; \
    flag &= ~mask

#endif

extern struct task_struct *current_task;
extern struct task_struct task_pool[];

void schedule_init();
int privilege_task_create(void (*func)());
void context_switch(struct task_struct *next);
void schedule();
void do_exec(void (*func)());
int do_fork();
void do_exit(int status);
extern void switch_to(struct cpu_context *prev, struct cpu_context *next);
