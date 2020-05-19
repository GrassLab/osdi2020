#ifndef TASK_H
#define TASK_H

#include <stdint.h>
#include "page.h"
#define QUEUE_ELE_SIZE 64
#define STACK_SIZE 4096
#define SIGKILL 1
typedef enum {
        INACTIVE,
        WAIT,
        ACTIVE,
        ZOMBIE,
} TASK_STATUS;


struct trap_frame_t {
    uint64_t x30;
    uint64_t x29;
    uint64_t x28;
    uint64_t x27;
    uint64_t x26;
    uint64_t x25;
    uint64_t x24;
    uint64_t x23;
    uint64_t x22;
    uint64_t x21;
    uint64_t x20;
    uint64_t x19;
    uint64_t x18;
    uint64_t x17;
    uint64_t x16;
    uint64_t x15;
    uint64_t x14;
    uint64_t x13;
    uint64_t x12;
    uint64_t x11;
    uint64_t x10;
    uint64_t x9;
    uint64_t x8;
    uint64_t x7;
    uint64_t x6;
    uint64_t x5;
    uint64_t x4;
    uint64_t x3;
    uint64_t x2;
    uint64_t x1;
    uint64_t x0;
};

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
    TASK_STATUS status;
    int time;
    int signal;
    int priority;
    int reschedule;
    uint64_t elr;
    uint64_t pgd;
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

extern struct task_t task_pool[64];
extern struct queue_element_t queue_elements[QUEUE_ELE_SIZE];
extern int queue_elements_now;
struct task_t* get_current();
struct task_t* privilege_task_create(void (*func)(), int priority);
extern struct queue runqueue;
extern struct queue waitqueue;
extern char kstack_pool[64][STACK_SIZE];
extern char ustack_pool[64][STACK_SIZE];

void context_switch(struct task_t* next);
void privilege_task_run();
void schedule();
void task_init();
void queue_push(struct queue* queue, struct task_t* task);
struct task_t* queue_pop(struct queue* queue, TASK_STATUS status);
void do_exec(void (*func)());
void do_fork(uint64_t elr);
void do_exit(uint64_t status);
void kexit(uint64_t status);
void do_kill(uint64_t pid, uint64_t signal);
void switch_to_wait();
#endif
