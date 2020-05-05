#include "schedule.h"
#include "mm.h"
#include "uart0.h"
#include "queue.h"
#include "exception.h"
#include "sysregs.h"
#include "util.h"
#include "sys.h"

struct runqueue runqueue;
struct task_struct task_pool[TASK_POOL_SIZE] = {INIT_TASK, };
struct task_struct *current_task;

void preempt_disable() {
    current_task->preemptable_flag = 0;
}

void preempt_enable() {
    current_task->preemptable_flag = 1;
}

void demo_priviledge() {
    while (1) {
        uart_printf("pid: %d\n", current_task->id);
        for (int i = 0; i < 100000; i++);
    }
}

void demo_do_exec_user_mode() {
    char buf[128];
    while(1) {
        uart_printf("[%f] hello from %d in user mode\n", get_timestamp(), current_task->id);
        sys_uart_read(buf, 5);
        sys_uart_write(buf, 5);
        for (int i = 0; i < 100000; i++);
    }
}

void demo_do_exec() {
    uart_printf("task %d do_exec\n", current_task->id);
    do_exec(demo_do_exec_user_mode);
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
        task_pool[i].reschedule_flag = 0;
        task_pool[i].preemptable_flag = 1;
        task_pool[i].priority = INIT_PRIORITY;
        task_pool[i].counter = task_pool[i].priority;
    }

    current_task = &task_pool[0];
    current_task->state = RUNNING;
    QUEUE_PUSH(runqueue, current_task);

    privilege_task_create(demo_do_exec);
    // privilege_task_create(demo_priviledge);
    // privilege_task_create(demo_priviledge);

    arm_core_timer_enable();
    schedule();
}

void context_switch(struct task_struct *next) {
    if (current_task->id != 0 && current_task->state != EXIT) {
        QUEUE_PUSH(runqueue, current_task);
    }

    if (current_task->id == next->id) return;

    struct task_struct* prev = current_task;
    current_task = next;
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

void do_exec(void (*func)()) {
    uint64_t user_stk_top = (uint64_t)(ustack_pool[current_task->id]);
    asm volatile("msr sp_el0, %0" : : "r"(user_stk_top));
    asm volatile("msr elr_el1, %0": : "r"(func));
    asm volatile("msr spsr_el1, %0" : : "r"(SPSR_EL1_VALUE));
    asm volatile("eret");
}
