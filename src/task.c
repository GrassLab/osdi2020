#include <io.h>
#include <stdint.h>
#include <task.h>

#include "page.h"
#include "utils.h"

struct task_t task_pool[64];
struct queue_element_t queue_elements[QUEUE_ELE_SIZE];
int queue_elements_now;
struct queue runqueue;
struct queue waitqueue;
char kstack_pool[64][STACK_SIZE];
char ustack_pool[64][STACK_SIZE];

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
        print_s(" has been killed\n");
        schedule();
    } else {
        switch_to(prev, next, nextfunc, next->spsr, next->pgd);
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

struct task_t* queue_pop(struct queue* queue, TASK_STATUS status) {
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
    uint64_t* pgd;
    asm volatile("mrs %0, ttbr1_el1" : "=r"(pgd));
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
            task_pool[i].signal = 0;
            task_pool[i].pgd = pgd;
            task_pool[i].pages_now = 0;
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
    uint64_t* pgd;
    asm volatile("mrs %0, ttbr1_el1" : "=r"(pgd));
    switch_to(&tmp, task, task->elr, task->spsr, pgd);
}

void schedule() {
    struct task_t* task = queue_pop(&runqueue, ACTIVE);
    print_s("\nschdule pid: ");
    print_i(task->id);
    print_s("\n");
    queue_push(&runqueue, task);
    context_switch(task);
}

void switch_to_user_mode() {
    struct task_t* task = get_current();
    uint64_t sp = task->utask.sp;
    uint64_t func = task->utask.elr;
    user_context(sp, func);
}
