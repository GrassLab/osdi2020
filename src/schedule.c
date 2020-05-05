#include "schedule.h"
#include "mm.h"
#include "uart0.h"
#include "queue.h"
#include "exception.h"

struct runqueue runqueue;
struct task_struct task_pool[TASK_POOL_SIZE] = {INIT_TASK, };
struct task_struct *current;

void preempt_disable() {
    current->flag &= !(1 << PREEMPTABLE_BIT);
}

void preempt_enable() {
    current->flag |= (1 << PREEMPTABLE_BIT);
}

void reshedule() {
    uart_printf("reschedule from %d\n", current->id);
    current->flag &= !(1 << RESHEDULE_BIT);
    current->counter = current->priority;
    schedule();
}

void task_demo() {
    while (1) {
        uart_printf("%d %d\n", current->id, current->flag);
        for (int i = 0; i < 100000; i++);
        if (RESHEDULE(current->flag)) {
            QUEUE_PUSH(runqueue, current);
            reshedule();
        }
    }
}

void privilege_task_create(void(*func)()) {
    struct task_struct *new_task;
    for (int i = 0; i < TASK_POOL_SIZE; i++) {
        if (task_pool[i].state == EXIT) {
            new_task = &task_pool[i];
            break;
        }
    }

    new_task->state = RUNNING;
    new_task->cpu_context.lr = (uint64_t)func;
    new_task->cpu_context.fp = (uint64_t)(kstack_pool[new_task->id]);
    new_task->cpu_context.sp = (uint64_t)(kstack_pool[new_task->id]);

    QUEUE_PUSH(runqueue, new_task);
}

void schedule_init() {
    QUEUE_INIT(runqueue, TASK_POOL_SIZE);

    for (int i = 0; i < TASK_POOL_SIZE; i++) {
        task_pool[i].id = i;
        task_pool[i].state = EXIT;
        task_pool[i].flag = INIT_FLAG;
        task_pool[i].priority = INIT_PRIORITY;
        task_pool[i].counter = task_pool[i].priority;
    }

    current = &task_pool[0];
    current->state = RUNNING;
    QUEUE_PUSH(runqueue, current);

    privilege_task_create(task_demo);
    privilege_task_create(task_demo);
    privilege_task_create(task_demo);

    arm_core_timer_enable();
    schedule();
}

void context_switch(struct task_struct *next) {
    if (current->id == next->id) return;

    struct task_struct* prev = current;
    current = next;
    switch_to(&prev->cpu_context, &next->cpu_context);
}

void schedule() {
    preempt_disable();
    struct task_struct* next = QUEUE_POP(runqueue);
    if (next->id == 0) {
        if (QUEUE_SIZE(runqueue) != 0) { // task idle is scheduled but there has other tasks
            next = QUEUE_POP(runqueue);
        }
        QUEUE_PUSH(runqueue, &task_pool[0]); // task idle must exist in runqueue
    }
    context_switch(next);
    preempt_enable();
}
