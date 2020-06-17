#include "peripherals/uart.h"
#include "peripherals/irq.h"
#include "sched.h"
#include "mm.h"

static struct task_struct init_task = INIT_TASK;
struct task_struct *current = &(init_task);
struct task_struct * tasks[NR_TASKS] = {&(init_task),};
unsigned int nr_tasks = 1;

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
        for (int i = 0; i < NR_TASKS; ++ i) {
            t = tasks[i];
            if (t && (t->state == TASK_RUNNING)) {
                if (t->counter > max_c) {
                    max_c = t->counter;
                    next = i;
                }
            }
        }

        if (max_c > 0) {
            break;
        }

        for (int i = 0; i < NR_TASKS; ++ i) {
            t = tasks[i];
            if (t && (t->state == TASK_RUNNING)) {
                /* TODO: why >> 1 */
                t->counter = t->counter + t->priority;;
            }
        }
    }
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

void exit_process() {
    // TODO: why
    preempt_disable();
    enable_irq();
    for (int i = 0; i < NR_TASKS; ++ i) {
        if (tasks[i] == current) {
            uart_puts("Task ");
            uart_send_ulong(current->task_id);
            uart_puts(" exit.\n");
            current->state = TASK_ZOMBIE;
            break;
        }
    }
    if (current->stack) {
        free_task_struct(current->stack);
    }    
    preempt_enable();
    schedule();
}
