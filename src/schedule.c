#include "schedule.h"
#include "mm.h"
#include "uart0.h"
#include "queue.h"
#include "exception.h"
#include "sysregs.h"
#include "util.h"
#include "sys.h"
#include "signal.h"

struct runqueue runqueue;
struct task_struct task_pool[TASK_POOL_SIZE];
struct task_struct *current_task;

void preempt_disable() {
    CLR(current_task->flag, PREEMPTABLE_BIT);
}

void preempt_enable() {
    SET(current_task->flag, PREEMPTABLE_BIT);
}

/* task can be executed */

void demo_sigkill_user() {
    int cnt = 3;
    int id = fork();
    uart_printf("task id: %d fork return: %d\n", current_task->id, id);
    if (id > 0) { // parent process
        while(cnt--) {
            uart_printf("Hello from parent %d\n", current_task->id);
            for(int i = 0; i < 100000; i++);
        }
        kill(id, SIG_KILL);
        exit(0);
    }
    else if (id == 0) { // child process
        while(1) {
            uart_printf("Hello from child %d\n", current_task->id);
            for(int i = 0; i < 100000; i++);
        }
    }
}

void demo_sigkill() {
    do_exec(demo_sigkill_user);
}

void demo_priviledge() {
    while (1) {
        uart_printf("pid: %d\n", current_task->id);
        for (int i = 0; i < 100000; i++);
    }
}

void demo_sys_exec() {
    int tmp = 5;
    uart_printf("Task %d after exec, tmp address 0x%x, tmp value %d\n", current_task->id, &tmp, tmp);
    exit(0);
}

void demo_do_exec_user_mode() {
    int cnt = 1;
    int id = fork();
    uart_printf("task id: %d fork return: %d\n", current_task->id, id);
    if (id > 0) { // parent process
        uart_printf("Message from parent %d\n", current_task->id);
        exec(demo_sys_exec);
    }
    else if (id == 0) { // child process
        fork();
        for (int i = 0; i < 100000; i++);
        fork();
        while (cnt < 10) {
            uart_printf("Task id: %d, cnt: %d\n", current_task->id, cnt);
            for (int i = 0; i < 100000; i++);
            ++cnt;
        }
        exit(0);
        uart_printf("Should not be printed\n");
    }
    else {
        uart_printf("Fork failed with code: %d", id);
    }
}

void demo_do_exec() {
    uart_printf("task %d do_exec\n", current_task->id);
    do_exec(demo_do_exec_user_mode);
}

/* scheduler */

int privilege_task_create(void(*func)()) {
    struct task_struct *new_task;
    int i;
    for (i = 0; i < TASK_POOL_SIZE; i++) {
        if (task_pool[i].state == EXIT) {
            new_task = &task_pool[i];
            break;
        }
    }
    if (i == TASK_POOL_SIZE) return -1; // can not allocate task struct

    // find avaliable kernel stack
    new_task->kstack = get_avaliable_kstack();
    if (!new_task->kstack) return -2; // can not allocate kernel stack

    new_task->cpu_context.lr = (uint64_t)func;
    new_task->cpu_context.fp = (uint64_t)(new_task->kstack);
    new_task->cpu_context.sp = (uint64_t)(new_task->kstack);

    if (QUEUE_FULL(runqueue)) return -3; // runqueue is full
    QUEUE_PUSH(runqueue, new_task);

    new_task->state = RUNNING;

    return new_task->id;
}

void zombie_reaper() {
    while (1) {
        for (int i = 0; i < TASK_POOL_SIZE; i++) {
            if (task_pool[i].state == ZOMBIE) {
                uart_printf("reaper %d!\n", i);
                task_pool[i].state = EXIT;
                // release kernel stack
                release_kstack(i);
            }
        }
        schedule();
    }
}

void schedule_init() {
    QUEUE_INIT(runqueue, TASK_POOL_SIZE);

    for (int i = 0; i < TASK_POOL_SIZE; i++) {
        task_pool[i].id = i;
        task_pool[i].state = EXIT;
        task_pool[i].flag = INIT_FLAG;
        task_pool[i].priority = INIT_PRIORITY;
        task_pool[i].counter = task_pool[i].priority;
    }

    // initial task
    current_task = &task_pool[0];
    current_task->state = RUNNING;
    current_task->kstack = (char*) KERNEL_BASE;
    QUEUE_PUSH(runqueue, current_task);
    privilege_task_create(zombie_reaper);

    // demo tasks
    privilege_task_create(demo_sigkill);
    // privilege_task_create(demo_do_exec);
    // privilege_task_create(demo_priviledge);
    // privilege_task_create(demo_priviledge);

    arm_core_timer_enable();
    schedule();
}

void context_switch(struct task_struct *next) {
    if (current_task->id != 0 && current_task->state == RUNNING) {
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
        while (next->state == ZOMBIE || next->state == EXIT) { // get runnable task
            next = QUEUE_POP(runqueue);
        }
        QUEUE_PUSH(runqueue, &task_pool[0]); // task idle must exist in runqueue
    }
    context_switch(next);
    preempt_enable();
}

void do_exec(void (*func)()) {
    current_task->ustack = get_avaliable_ustack();
    asm volatile("msr sp_el0, %0" : : "r"(current_task->ustack));
    asm volatile("msr elr_el1, %0": : "r"(func));
    asm volatile("msr spsr_el1, %0" : : "r"(SPSR_EL1_VALUE));
    asm volatile("eret");
}

void do_exit(int status) {
    current_task->state = ZOMBIE;
    current_task->exit_status = status;

    // release user stack
    release_ustack(current_task->id);
    schedule();
}
