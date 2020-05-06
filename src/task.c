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
    if (next->signal == SIGKILL) {
        next->status = ZOMBIE;
        print_s("pid ");
        print_i(next->id);
        print_s("has been killed\n");
        schedule();
    } else {
        switch_to(prev, next, nextfunc, next->spsr);
    }
}

void queue_push(struct queue* queue, struct task_t* task) {
    struct queue_element_t* queue_element = &queue_elements[queue_elements_now];
    queue_elements_now++;
    queue_element->task = task;
    if (queue->head == 0) {
        queue->tail = queue_element;
        queue->head = queue_element;
    } else if (queue->head->task->priority > task->priority) {
        queue_element->next = queue->head;
        queue->head = queue_element;
    } else {
        struct queue_element_t* start = queue->head;
        while (start->next != 0 &&
               start->next->task->priority <= task->priority) {
            start = start->next;
        }
        queue_element->next = start->next;
        start->next = queue_element;
        start = queue->head;
        while (start != 0) {
            start = start->next;
        }
    }
}

struct task_t* queue_pop(struct queue* queue, int status) {
    struct task_t* task;
    if (queue->head == 0) {
        return &task_pool[0];
    } else if (queue->head == queue->tail) {
        task = queue->head->task;
        queue->head = 0;
        queue->tail = 0;
        if (task->status != status) {
            return queue_pop(&runqueue, status);
        } else {
            return task;
        }
    } else {
        task = queue->head->task;
        queue->head = queue->head->next;
        if (task->status != status) {
            return queue_pop(&runqueue, status);
        } else {
            return task;
        }
    }
}

struct task_t* privilege_task_create(void (*func)(), int priority) {
    struct task_t* task = 0;
    uint64_t spsr_el1;
    for (int i = 0; i < 64; i++) {
        if (task_pool[i].status == INACTIVE) {
            task = &task_pool[i];
            task_pool[i].id = i;
            task_pool[i].sp = (uint64_t)kstack_pool[i + 1];
            task_pool[i].elr = (uint64_t)func;
            task_pool[i].time = 0;
            asm volatile("mrs %0, spsr_el1" : "=r"(spsr_el1));
            task_pool[i].spsr = spsr_el1;
            task_pool[i].priority = priority;
            task_pool[i].status = ACTIVE;
            break;
        }
    }
    queue_push(&runqueue, task);
    return task;
}

void task_init() {
    runqueue.head = 0;
    runqueue.tail = 0;
}

void privilege_task_run() {
    struct task_t* task = queue_pop(&runqueue, ACTIVE);
    struct task_t tmp;
    print_s("schdule pid: ");
    print_i(task->id);
    print_s("\n");
    queue_push(&runqueue, task);
    switch_to(&tmp, task, task->elr, task->spsr);
}

void schedule() {
    struct task_t* task = queue_pop(&runqueue, ACTIVE);
    print_s("schdule pid: ");
    print_i(task->id);
    print_s("\n");
    queue_push(&runqueue, task);
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
            task_pool[i].priority = task->priority;
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
    queue_push(&runqueue, child_task);
}

void kexit(uint64_t status) {
    do_exit(status);
    schedule();
}

void do_exit(uint64_t status) {
    struct task_t* task = get_current();
    task->status = ZOMBIE;

    print_s("Exited with status code: ");
    print_i(status);
    print_s("\n");
}

void do_kill(uint64_t pid, uint64_t signal) { task_pool[pid].signal = signal; }
