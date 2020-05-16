#include "schedule.h"
#include "uart0.h"
#include "demo.h"

struct task_t task_pool[TASK_POOL_SIZE];
char kstack_pool[TASK_POOL_SIZE][KSTACK_SIZE];

void schedule_init() {
    for (int i = 0; i < TASK_POOL_SIZE; i++) {
        task_pool[i].id = i;
        task_pool[i].state = EXIT;
    }

    task_pool[0].state = RUNNING;
    update_current_task(&task_pool[0]);

    privilege_task_create(demo_task_1);
    privilege_task_create(demo_task_2);

    context_switch(&task_pool[1]);
}

void privilege_task_create(void (*func)()) {
    struct task_t *new_task;
    for (int i = 0; i < TASK_POOL_SIZE; i++) {
        if (task_pool[i].state == EXIT) {
            new_task = &task_pool[i];
            break;
        }
    }

    new_task->state = RUNNING;
    new_task->cpu_context.lr = (uint64_t)func;
    new_task->cpu_context.fp = (uint64_t)(&kstack_pool[new_task->id][TASK_POOL_SIZE - 1]);
    new_task->cpu_context.sp = (uint64_t)(&kstack_pool[new_task->id][TASK_POOL_SIZE - 1]);
}

void context_switch(struct task_t* next) {
    struct task_t* prev = get_current_task();
    update_current_task(next);
    switch_to(&prev->cpu_context, &next->cpu_context);
}
