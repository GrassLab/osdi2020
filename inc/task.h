#ifndef TASK_H
#define TASK_H

#include <stdint.h>
#define RUNQUEUE_SIZE 64

struct utask_t {
    uint64_t context[10];
    uint64_t spsr;
    uint64_t elr;
    uint64_t sp;
    void (*func)();
};

struct task_t {
    uint64_t context[10];
    uint64_t fp;
    uint64_t lr;
    uint64_t sp;
    int id;
    int used;
    int time;
    int reschedule;
    struct utask_t utask;
    void (*func)();
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
char kstack_pool[64][4096];
char ustack_pool[64][4096];

void context_switch(struct task_t* next);
void privilege_task_run(struct task_t* this_task);
void schedule();
void task_init();
void runqueue_push(struct task_t* task);
void do_exec(void (*func)());
#endif
