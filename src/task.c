#include <io.h>
#include <stdint.h>
#include <task.h>
#define current get_current();

extern void switch_to(struct task_t* prev, struct task_t* next,
                      uint64_t nextfunc);

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

char kstack_pool[64][4096];
char ustack_pool[64][4096];

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

void switch_to_user_mode(void (*func)()) {
    asm volatile("tt:");
    asm volatile("mov     x0, %0" : "=r"(func));
    asm volatile("msr     elr_el1, x0");
    asm volatile("bl      set_aux");
    asm volatile("eret");
}

void do_exec(void (*func)()) { switch_to_user_mode(func); }
