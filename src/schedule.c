#include "schedule.h"
#include "mm.h"
#include "uart0.h"
#include "queue.h"

struct uint64_queue runqueue;
struct task_struct task_pool[TASK_POOL_SIZE] = {INIT_TASK, };
struct task_struct *current;

void task_demo() {
    while (1) {
        uart_printf("hi\n");
        for (int i = 0; i < 5000000; i++);
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

    QUEUE_PUSH(runqueue, new_task->id);
}

void schedule_init() {
    QUEUE_INIT(runqueue, TASK_POOL_SIZE);

    for (int i = 0; i < TASK_POOL_SIZE; i++) {
        task_pool[i].id = i;
        task_pool[i].state = EXIT;
    }

    current = &task_pool[0];
    current->state = RUNNING;
    QUEUE_PUSH(runqueue, 0);

    privilege_task_create(task_demo);
    context_switch(&task_pool[1]);
}

void context_switch(struct task_struct *next) {
    struct task_struct* prev = current;
    current = next;
    switch_to(&prev->cpu_context, &next->cpu_context);
}
