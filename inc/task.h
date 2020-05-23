#ifndef TASK_H
#define TASK_H

#include <stdint.h>
#include "trap.h"
#define QUEUE_ELE_SIZE 64
#define STACK_SIZE 4096
#define SIGKILL 1
typedef enum {
        INACTIVE,
        WAIT,
        ACTIVE,
        ZOMBIE,
} TASK_STATUS;


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
    struct trap_frame_t *trap_frame;
    uint64_t* pgd;
    uint64_t user_page;
    uint64_t stack_page;
    uint64_t pages[100];
    uint64_t pages_now;
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
extern struct queue runqueue;
extern struct queue waitqueue;
extern int queue_elements_now;
extern char kstack_pool[64][STACK_SIZE];
extern char ustack_pool[64][STACK_SIZE];

extern void switch_to(struct task_t* prev, struct task_t* next,
                      uint64_t nextfunc, uint64_t spsr, uint64_t* pgd);
extern void user_context(uint64_t sp, uint64_t func);
struct task_t* get_current();
void context_switch(struct task_t* next);
void queue_push(struct queue* queue, struct task_t* task);
struct task_t* queue_pop(struct queue* queue, TASK_STATUS status);
struct task_t* privilege_task_create(void (*func)(), int priority);
void task_init();
void privilege_task_run();
void schedule();
void switch_to_user_mode();
#endif
