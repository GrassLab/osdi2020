#ifndef TASK_H
#define TASK_H

#include <stdint.h>
#define RUNQUEUE_SIZE 64
#define STACK_SIZE 4096

struct utask_t {
    uint64_t sp;
    uint64_t elr;
    uint64_t fork_id;
};

struct task_t {
    uint64_t context[10];
    uint64_t fp;
    uint64_t lr;
    uint64_t sp;
    struct utask_t utask;
    uint64_t spsr;
    int id;
    int used;
    int status;
    int time;
    int killed;
    int reschedule;
    uint64_t elr;
};

struct queue_element_t {
    struct task_t* task;
    int is_active;
};

struct task_t task_pool[64];
struct queue_element_t runqueue[RUNQUEUE_SIZE];
int runqueue_now;
int runqueue_last;
struct task_t* get_current();
struct task_t* privilege_task_create(void (*func)());
char kstack_pool[64][STACK_SIZE];
char ustack_pool[64][STACK_SIZE];

void context_switch(struct task_t* next);
void privilege_task_run(struct task_t* this_task);
void schedule();
void task_init();
void runqueue_push(struct task_t* task);
void do_exec(void (*func)());
void do_fork(uint64_t elr);
void do_exit(uint64_t status);
#endif
