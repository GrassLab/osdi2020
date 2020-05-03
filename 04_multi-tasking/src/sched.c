#include "sched.h"

static struct task_struct init_task = INIT_TASK;
struct task_struct *current = &(init_task);
struct task_struct * tasks[MAX_CONCURRENT_TASKS] = {&(init_task),};
unsigned int num_tasks = 1;

/* 0 is for init_task */
static tid_t used_task_id = 0;

tid_t acquire_unused_task_id() {
    return ++ used_task_id;
}

void switch_to(struct task_struct *next) {
    if (current == next)
        return;
    struct task_struct *prev = current;
    current = next;
    cpu_switch(prev, next);
}

void schedule() {
    preempt_disable();
    struct task_struct *t;
    int next, max_c;
    
    while (1) {
        max_c = -1;
        next = 0;

        /* Find the task with maximum token */
        for (int i = 0; i < MAX_CONCURRENT_TASKS; ++ i) {
            t = tasks[i];
            if (t && (t->state == TASK_RUNNING)) {
                /*
                uart_send_ulong(i);
                uart_send(':');
                uart_send_ulong(t->counter);
                uart_send(';');
                */
                if (t->counter > max_c) {
                    max_c = t->counter;
                    next = i;
                }
            }
        }
        // uart_send('\n');

        if (max_c > 0) {
            break;
        }

        for (int i = 0; i < MAX_CONCURRENT_TASKS; ++ i) {
            t = tasks[i];
            if (t) {
                /* TODO: why >> 1 */
                t->counter = t->counter + t->priority;;
            }
        }
    }
    /*
    uart_puts("Preparing switch to task[");
    uart_send_ulong(next);
    uart_puts("]\n");
    */
    switch_to(tasks[next]);
    preempt_enable();
}

void schedule_tail() {
    preempt_enable();
}

inline void preempt_disable() {
    current->preempt_count ++;
}

inline void preempt_enable() {
    current->preempt_count --;
}
