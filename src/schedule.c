#include "schedule.h"
#include "uart0.h"
#include "demo.h"
#include "task_queue.h"
#include "exception.h"
#include "sysregs.h"

struct task_t task_pool[TASK_POOL_SIZE];
char kstack_pool[TASK_POOL_SIZE][KSTACK_SIZE];
char ustack_pool[TASK_POOL_SIZE][USTACK_SIZE];
struct task_queue_elmt_t runqueue_elmt_pool[TASK_POOL_SIZE];
struct task_queue_t runqueue;

void runqueue_init() {
    for (int i = 0; i < TASK_POOL_SIZE; i++) {
        task_queue_elmt_init(&runqueue_elmt_pool[i], &task_pool[i]);
    }
    task_queue_init(&runqueue);
}

struct task_queue_elmt_t* get_runqueue_elmt(struct task_t* task) {
    return &runqueue_elmt_pool[task->id];
}

/* scheduler */

void task_init() {
    for (int i = 0; i < TASK_POOL_SIZE; i++) {
        task_pool[i].id = i;
        task_pool[i].state = EXIT;
    }
    // idle task
    task_pool[0].state = RUNNING;
    task_pool[0].priority = 0;
    update_current_task(&task_pool[0]);
}

void schedule_init() {
    runqueue_init();

    // privilege_task_create(demo_task_1, 10);
    // privilege_task_create(demo_task_2, 10);
    // privilege_task_create(demo_do_exec, 15);
    privilege_task_create(demo_syscall, 10);

    arm_core_timer_enable();
    schedule();
}

int privilege_task_create(void (*func)(), int priority) {
    struct task_t *new_task;
    for (int i = 0; i < TASK_POOL_SIZE; i++) {
        if (task_pool[i].state == EXIT) {
            new_task = &task_pool[i];
            break;
        }
    }

    new_task->state = RUNNING;
    new_task->priority = priority;
    new_task->counter = TASK_EPOCH;
    new_task->need_resched = 0;
    new_task->cpu_context.lr = (uint64_t)func;
    new_task->cpu_context.fp = (uint64_t)(&kstack_pool[new_task->id][KSTACK_SIZE - 1]);
    new_task->cpu_context.sp = (uint64_t)(&kstack_pool[new_task->id][KSTACK_SIZE - 1]);

    task_queue_push(&runqueue, get_runqueue_elmt(new_task));

    return new_task->id;
}

void context_switch(struct task_t* next) {
    struct task_t* prev = get_current_task();
    task_queue_push(&runqueue, get_runqueue_elmt(prev));
    update_current_task(next);
    switch_to(&prev->cpu_context, &next->cpu_context);
}

void schedule() {
    struct task_t* next = task_queue_pop(&runqueue);
    context_switch(next);
}

void reschedule() {
    struct task_t *current = get_current_task();
    if (current->need_resched) {
        current->counter = TASK_EPOCH;
        current->need_resched = 0;
        schedule();
    }
}

void do_exec(void (*func)()) {
    struct task_t *current = get_current_task();
    asm volatile("msr sp_el0, %0" : : "r"(&ustack_pool[current->id][USTACK_SIZE - 1]));
    asm volatile("msr elr_el1, %0": : "r"(func));
    asm volatile("msr spsr_el1, %0" : : "r"(SPSR_EL1_VALUE));
    asm volatile("eret");
}
