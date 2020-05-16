#include "schedule.h"

struct task_t *current;
struct task_t task_pool[TASK_POOL_SIZE];
char kstack_pool[TASK_POOL_SIZE][KSTACK_SIZE];

void schedule_init() {
    for (int i = 0; i < TASK_POOL_SIZE; i++) {
        task_pool[i].id = i;
        task_pool[i].state = EXIT;
    }
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
    new_task->cpu_context.fp = (uint64_t)(kstack_pool[new_task->id]);
    new_task->cpu_context.sp = (uint64_t)(kstack_pool[new_task->id]);
}
