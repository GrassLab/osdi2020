#include "typedef.h"

#ifndef __SCHEDULE_H__
#define __SCHEDULE_H__

#define TASK_EPOCH 5
#define TASK_POOL_SIZE 64
#define KSTACK_SIZE 4096
#define USTACK_SIZE 4096
#define KSTACK_TOP_IDX (KSTACK_SIZE - 16) // sp need 16bytes alignment
#define USTACK_TOP_IDX (USTACK_SIZE - 16) // sp need 16bytes alignment

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

struct task_t {
    uint64_t id;
    enum task_state state;
    int priority;
    int counter;
    int need_resched;
    int exit_status;
    struct cpu_context cpu_context;
};

/* Variables init in schedule.c */
extern struct task_t task_pool[TASK_POOL_SIZE];
extern char kstack_pool[TASK_POOL_SIZE][KSTACK_SIZE];
extern char ustack_pool[TASK_POOL_SIZE][USTACK_SIZE];

/* Function in schedule.S */
extern struct task_t* get_current_task();
extern void update_current_task(struct task_t *task);
extern void switch_to(struct cpu_context* prev, struct cpu_context* next);

/* Function in schedule.c */
void task_init();
void schedule_init();
int privilege_task_create(void (*func)(), int priority);
void context_switch(struct task_t* next);
void schedule();
void do_exec(void (*func)());
void do_exit(int status);

#endif
