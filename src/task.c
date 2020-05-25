#include <io.h>
#include <stdint.h>
#include <task.h>

#include "page.h"
#include "utils.h"
#define current get_current();

extern void switch_to(struct task_t* prev, struct task_t* next,
                      uint64_t nextfunc, uint64_t spsr, uint64_t* pgd);
extern void user_context(uint64_t sp, uint64_t func);

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
        asm volatile("e:");
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

struct utask_t* get_current_utask() {
    uint64_t res;
    asm volatile("mrs %0, tpidr_el0" : "=r"(res));
    struct utask_t* res_task = (struct utask_t*)res;

    return res_task;
}

void switch_to_user_mode() {
    struct task_t* task = get_current();
    uint64_t sp = task->utask.sp;
    uint64_t func = task->utask.elr;
    user_context(sp, func);
}

void do_exec(uint8_t* func, int size) {
    struct task_t* task = get_current();

    struct page_t* user_page = page_alloc();
    task->pages[task->pages_now++] = user_page->id;
    asm volatile("b:");
    for (int i = 0; i < size; i++) {
        *((uint8_t*)user_page->content + i) = *(func + i);
    }
    page_mapping(task, user_page);
    task->utask.elr = 0;

    struct page_t* stack_page = page_alloc();
    task->pages[task->pages_now++] = stack_page->id;
    stack_mapping(task, stack_page);
    task->utask.sp = 0x0000ffffffffe000;

    asm volatile("pgd:");
    uint64_t utask_addr = (uint64_t)&task->utask;
    move_ttbr(task->pgd);

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
            struct page_t* user_page = page_alloc();
            task_pool[i].pages[task_pool[i].pages_now++] = user_page->id;

            memcpy(user_page->content, pages[task->user_page].content,
                   4 * 1024);

            page_mapping(&task_pool[i], user_page);
            task_pool[i].utask.elr = 0;

            struct page_t* stack_page = page_alloc();
            task_pool[i].pages[task_pool[i].pages_now++] = stack_page->id;
            stack_mapping(&task_pool[i], stack_page);

            memcpy(stack_page->content, pages[task->stack_page].content,
                   4 * 1024);

            task_pool[i].id = i;
            task_pool[i].elr = elr;
            task_pool[i].time = 0;
            task_pool[i].pages_now = 0;
            task_pool[i].spsr = task->spsr;
            task_pool[i].priority = task->priority;
            task_pool[i].status = ACTIVE;
            task_pool[i].utask.elr = task->utask.elr;
            task_pool[i].utask.sp = task->utask.sp;
            task_pool[i].sp = task_pool[i].utask.sp;
            task_pool[i].signal = 0;
            task_pool[i].utask.fork_id = 0;
            /* task_pool[i].trap_frame->x0 = 0; */
            memcpy(&kstack_pool[i - 1] + 1, &kstack_pool[task->id - 1] + 1,
                   STACK_SIZE * sizeof(char));
            task->utask.fork_id = task_pool[i].id;
            task->trap_frame->x0 = task_pool[i].id;
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
    for (uint64_t i = 0; i < task->pages_now - 1; i++) {
        page_free(&pages[task->pages[i]]);
    }
    print_s("Exited with status code: ");
    print_i(status);
    print_s("\n");
}

void do_kill(uint64_t pid, uint64_t signal) { task_pool[pid].signal = signal; }
