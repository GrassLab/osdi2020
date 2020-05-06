#ifndef TASK_H
#define TASK_H

#include <stdint.h>
#define QUEUE_ELE_SIZE 64
#define STACK_SIZE 4096
#define SIGKILL 1
#define INACTIVE 0
#define WAIT 0
#define ACTIVE 1
#define ZOMBIE 2

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
    int status;
    int time;
    int signal;
    int priority;
    int reschedule;
    uint64_t uart_sp;
    uint64_t uart_lr;
    uint64_t uart_elr;
    uint64_t elr;
};

struct queue_element_t {
    struct task_t* task;
    struct queue_element_t* next ;
    int is_active;
};

struct queue{
    struct queue_element_t* head;
    struct queue_element_t* tail;
};

struct task_t task_pool[64];
struct queue_element_t queue_elements[QUEUE_ELE_SIZE];
int queue_elements_now;
struct task_t* get_current();
struct task_t* privilege_task_create(void (*func)(), int priority);
struct queue runqueue;
struct queue waitqueue;
char kstack_pool[64][STACK_SIZE];
char ustack_pool[64][STACK_SIZE];

void context_switch(struct task_t* next);
void privilege_task_run();
void schedule();
void task_init();
void queue_push(struct queue* queue, struct task_t* task);
struct task_t* queue_pop(struct queue* queue, int status);
void do_exec(void (*func)());
void do_fork(uint64_t elr);
void do_exit(uint64_t status);
void kexit(uint64_t status);
void do_kill(uint64_t pid, uint64_t signal);
void switch_to_wait();
#endif
