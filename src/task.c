#include <io.h>
#include <stdint.h>
#include <task.h>
#define current get_current();

extern void switch_to(struct task_t* prev, struct task_t* next,
                      uint64_t nextfunc);
extern void user_context(uint64_t sp, uint64_t func);

struct task_t* get_current() {
    uint64_t res;
    asm volatile("mrs %0, tpidr_el1" : "=r"(res));
    struct task_t* res_task = (struct task_t*)res;

    return res_task;
}

void context_switch(struct task_t* next) {
    uint64_t nextfunc = (uint64_t)next->func;
    struct task_t* prev = get_current();
    switch_to(prev, next, nextfunc);
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
            runqueue_now = (now + 1) % RUNQUEUE_SIZE;
            return task;
        }
    }
    return &task_pool[0];
}

struct task_t* privilege_task_create(void (*func)()) {
    struct task_t* task = 0;
    for (int i = 0; i < 64; i++) {
        if (!task_pool[i].used) {
            task = &task_pool[i];
            task_pool[i].id = i;
            task_pool[i].sp = (uint64_t)kstack_pool[i];
            task_pool[i].func = func;
            task_pool[i].reschedule = 0;
            task_pool[i].used = 1;
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
    this_task->func();
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
    uint64_t func = (uint64_t)utask->func;
    user_context(sp, func);
}

void do_exec(void (*func)()) {
    struct task_t* task = get_current();
    task->utask.func = func;
    task->utask.sp = (uint64_t)ustack_pool[task->id];
    uint64_t utask_addr = (uint64_t)&task->utask;
    asm volatile("mov     x6, %0" : "=r"(utask_addr));
    asm volatile("msr     tpidr_el0, x6");
    asm volatile("ldr x2, =switch_to_user_mode");
    asm volatile("msr     elr_el1, x2");
    asm volatile("bl      set_aux");
    asm volatile("eret");
}
