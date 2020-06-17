#include "io.h"
#include "page.h"
#include "syscall.h"
#include "task.h"
#include "uart.h"
#include "utils.h"

void do_exec(uint8_t* func, int size) {
    struct task_t* task = get_current();
    struct page_t* user_page = page_alloc();
    task->pages[task->pages_now++] = user_page->id;
    for (int i = 0; i < size; i++) {
        *((uint8_t*)user_page->content + i) = *(func + i);
    }
    page_mapping(task, user_page);
    task->utask.elr = 0;

    struct page_t* stack_page = page_alloc();
    task->pages[task->pages_now++] = stack_page->id;
    stack_mapping(task, stack_page);
    /* task->utask.sp = 0x0000ffffffffe000; */
    task->utask.sp = 0x0000fffffffff000;

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
            memcpy(task_pool[i].context, task->context,
                   sizeof(task_pool[i].context));
            task_pool[i].fp = task->fp;
            task_pool[i].lr = task->lr;
            task_pool[i].utask.fork_id = 0;
            task_pool[i].utask.elr = task->utask.elr;
            task_pool[i].utask.sp = task->utask.sp;
            task_pool[i].sp = task_pool[i].utask.sp;
            task_pool[i].spsr = task->spsr;
            task_pool[i].id = i;
            task_pool[i].status = ACTIVE;
            task_pool[i].time = 0;
            task_pool[i].signal = 0;
            task_pool[i].priority = task->priority;

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

            task_pool[i].elr = elr;
            task_pool[i].pages_now = 0;
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

void syscall_handler(int syscall, struct trap_frame_t* trap_frame,
                     uint64_t elr) {
    char tmp;
    switch (syscall) {
        case SYS_PRINT:
            for (uint64_t i = 0; i < trap_frame->x1; i++) {
                uart_send(*((char*)trap_frame->x0 + i));
            }
            break;
        case SYS_SCAN:
            for (uint64_t i = 0; i < trap_frame->x1; i++) {
                *((char*)trap_frame->x0 + i) = uart_getc();
            }
            break;
        case SYS_EXEC:
            /* do_exec((void (*)())trap_frame->x0); */
            break;
        case SYS_FORK:
            do_fork(elr);
            break;
        case SYS_EXIT:
            do_exit(trap_frame->x0);
            asm volatile("ldr x0, =schedule");
            asm volatile("msr elr_el1, x0");
            break;
        case SYS_KILL:
            do_kill(trap_frame->x0, trap_frame->x1);
            break;
        case SYS_READ:
            tmp = uart_getc();
            trap_frame->x0 = tmp;
            break;
    }
}
