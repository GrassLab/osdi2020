#include <io.h>
#include <stdint.h>
#include <task.h>

#include "utils.h"
#define current get_current();

extern void switch_to(struct task_t* prev, struct task_t* next,
                      uint64_t nextfunc, uint64_t spsr);
extern void user_context(uint64_t sp, uint64_t func);

struct task_t* get_current() {
    uint64_t res;
    asm volatile("mrs %0, tpidr_el1" : "=r"(res));
    struct task_t* res_task = (struct task_t*)res;

    return res_task;
}

void context_switch(struct task_t* next) {
    uint64_t nextfunc = next->elr;
    struct task_t* prev = get_current();
    switch_to(prev, next, nextfunc, next->spsr);
}

void runqueue_push(struct task_t* task) {
    runqueue[runqueue_last].task = task;
    runqueue[runqueue_last].is_active = 1;
    runqueue_last = (runqueue_last + 1) % RUNQUEUE_SIZE;
}

struct task_t* runqueue_pop() {
    struct task_t* task;
    int now;
    for (int i = 0; i < RUNQUEUE_SIZE; i++) {
        now = (runqueue_now + i) % RUNQUEUE_SIZE;
        if (runqueue[now].is_active) {
            /* runqueue[runqueue_now].is_active = 0; */
            task = runqueue[now].task;
            if (task->status != ACTIVE) {
                runqueue[now].is_active = 0;
                continue;
            }
            runqueue_now = (now + 1) % RUNQUEUE_SIZE;
            return task;
        }
    }
    return &task_pool[0];
}

struct task_t* privilege_task_create(void (*func)()) {
    struct task_t* task = 0;
    uint64_t spsr_el1;
    for (int i = 0; i < 64; i++) {
        if (task_pool[i].status != ACTIVE) {
            task = &task_pool[i];
            task_pool[i].id = i;
            task_pool[i].sp = (uint64_t)kstack_pool[i + 1];
            task_pool[i].elr = (uint64_t)func;
            task_pool[i].time = 0;
            asm volatile("mrs %0, spsr_el1" : "=r"(spsr_el1));
            task_pool[i].spsr = spsr_el1;
            task_pool[i].reschedule = 0;
            task_pool[i].status = ACTIVE;
            break;
        }
    }
    runqueue_push(task);
    return task;
}

void task_init() {
    runqueue_now = 0;
    runqueue_last = 0;
}

void privilege_task_run(struct task_t* this_task) {
    asm volatile("msr tpidr_el1, %0" : "=r"(this_task));
    ((void (*)())this_task->elr)();
}

void schedule() {
    struct task_t* task = runqueue_pop();
    context_switch(task);
}

struct utask_t* get_current_utask() {
    uint64_t res;
    asm volatile("mrs %0, tpidr_el0" : "=r"(res));
    struct utask_t* res_task = (struct utask_t*)res;

    return res_task;
}

void switch_to_user_mode() {
    struct utask_t* utask = get_current_utask();
    uint64_t sp = utask->sp;
    uint64_t func = utask->elr;
    user_context(sp, func);
}

void do_exec(void (*func)()) {
    struct task_t* task = get_current();
    task->utask.elr = (uint64_t)func;
    task->utask.sp = (uint64_t)ustack_pool[task->id + 1];
    uint64_t utask_addr = (uint64_t)&task->utask;
    asm volatile("mov     x6, %0" : "=r"(utask_addr));
    asm volatile("msr     tpidr_el0, x6");
    asm volatile("ldr x2, =switch_to_user_mode");
    asm volatile("msr     elr_el1, x2");
    asm volatile("bl      set_aux");
    asm volatile("eret");
}

void do_fork(uint64_t elr) {
    struct task_t *task = get_current(), *child_task = 0;
    uint64_t sp_el0;
    asm volatile("mrs %0, sp_el0" : "=r"(sp_el0));
    task->utask.sp = sp_el0;
    for (int i = 0; i < 64; i++) {
        if (task_pool[i].status != ACTIVE) {
            child_task = &task_pool[i];
            task_pool[i].id = i;
            asm volatile("gg:");
            /* task_pool[i].sp = task->sp - (uint64_t)&kstack_pool[task->id + 1]
             * + */
            /* (uint64_t)&kstack_pool[i + 1]; */
            task_pool[i].elr = elr;
            task_pool[i].time = 0;
            task_pool[i].spsr = task->spsr;
            task_pool[i].reschedule = 0;
            task_pool[i].status = ACTIVE;
            task_pool[i].utask.elr = task->utask.elr;
            task_pool[i].utask.sp =
                (uint64_t)&ustack_pool[i + 1] -
                ((uint64_t)&ustack_pool[task->id + 1] - sp_el0);
            task_pool[i].sp = task_pool[i].utask.sp;
            task_pool[i].utask.fork_id = 0;
            memcpy(&kstack_pool[i - 1] + 1, &kstack_pool[task->id - 1] + 1,
                   STACK_SIZE * sizeof(char));
            memcpy(&ustack_pool[i - 1] + 1, &ustack_pool[task->id - 1] + 1,
                   STACK_SIZE * sizeof(char));
            asm volatile("gg2:");
            task->utask.fork_id = task_pool[i].id;
            break;
        }
    }
    runqueue_push(child_task);
}

void kexit(uint64_t status) {
    do_exit(status);

    schedule();
}

void do_exit(uint64_t status) {
    struct task_t* task = get_current();
    task->status = ZOMBIE;
    task->status = status;

    print_s("Exited with status code: ");
    print_i(status);
    print_s("\n");
}
