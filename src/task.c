#include "../include/uart.h"
#include "../include/time.h"
#include "../include/task.h"
#include "../include/queue.h"
#include "../include/syscall.h"


#define INIT_TASK \
{ \
/* cpu_context */   {0,0,0,0,0,0,0,0,0,0,0,0,0}, \
/* user_context */   {0,0,0}, \
/* state etc */	    0,0,0,0,0 \
}

struct task init_task = INIT_TASK;
struct task_manager TaskManager;

extern struct task* get_current();
extern void set_current(struct task* task_struct);
// struct task *current = &(init_task);


extern void switch_to(struct task* prev, struct task* next);
extern void ret_from_fork();

void task_manager_init()
{
    // uart_memset(TaskManager.kstack_pool, '\0', 64*4096);
    set_current(&(init_task));
    TaskManager.queue_bitmap = 0;
    TaskManager.task_num = 0;
}

void privilege_task_create(void(*func)())
{
    // assign the task id
    unsigned int task_id = TaskManager.task_num;

    struct task* new_task = &TaskManager.task_pool[task_id]; 
    // uart_hex((unsigned long) new_task);
    new_task->cpu_context.x19 = (unsigned long) func;
    new_task->cpu_context.x20 = task_id;
    // uart_hex((unsigned long) func);
    new_task->cpu_context.pc = (unsigned long)ret_from_fork;
    new_task->cpu_context.sp = (unsigned long) &TaskManager.kstack_pool[task_id];

    new_task->user_context.sp_el0 = &TaskManager.ustack_pool[task_id];
    new_task->user_context.spsr_el1 = 0;
    new_task->user_context.elr_el1 = 0;
    
    new_task->task_id = task_id;
    new_task->reschedule_flag = 0;
    new_task->state = RUN_IN_KERNEL_MODE;
    TaskManager.task_num++;

    // put the task into the runqueue
    // QUEUE_SET(RunQueue, (unsigned long) new_task);
    // QUEUE_PUSH(RunQueue);
}

void context_switch(struct task* next)
{
    struct task* current = get_current();
    struct task* prev = current;
    switch_to(prev, next);
}

void schedule()
{
    for(int i = 0; i < N; ++i) { // N should > 2
        struct task* next = &TaskManager.task_pool[i];
        next->counter = 3;
        context_switch(next);
    }
    // while (!QUEUE_EMPTY(RunQueue)) {
    //     struct task* next = QUEUE_GET(RunQueue);
    //     QUEUE_POP(RunQueue);

    //     QUEUE_SET(RunQueue, (unsigned long) next);
    //     QUEUE_PUSH(RunQueue);

    //     context_switch(next);
    // }

    return;
}


/* 
 ** kernel porcess for test
 */
void kernel_test(unsigned int task_id)
{
    while(1) {
        struct task* current = get_current();
        uart_puts("Task_id: ");
        // uart_hex(current->task_id);
        uart_hex(task_id);
        uart_puts("\n");
            
        // delay(1000000);
        wait_cycles(1000000);
        if (current->reschedule_flag == 1) {
            uart_puts("reschedule...\n");
            current->reschedule_flag = 0;
            schedule();
        }
    }
}

void idle()
{
    while(1){
        schedule();
        // delay(1000000);
        uart_puts("idle...\n");
        wait_cycles(1000000);
    }
}

/* 
 ** user porcess for test
 */
void user_test()
{
    do_exec(foo);
}
/*
 ** takes a function pointer to user code
 */
void do_exec(void(*func)())
{
    // be triggered at the first time execute
    struct task* current = get_current();
    uart_puts("do_exec$ Task_id: ");
    uart_hex(current->task_id);
    uart_puts("\n");

    // _setup_user_content(func);

    if (current->reschedule_flag == 1) {
        uart_puts("kernel routine reschedule...\n");
        current->reschedule_flag = 0;
        schedule();
    }
    if (current->state == RUN_IN_KERNEL_MODE) {
        // return to user mode
        current->state = RUN_IN_USER_MODE;
        unsigned long user_stack = current->user_context.sp_el0;
        unsigned long user_cpu_state = 0x0;
        asm volatile("msr sp_el0, %0" :: "r" (user_stack));
        asm volatile("msr spsr_el1, %0" :: "r" (user_cpu_state));
        asm volatile("msr elr_el1, %0" :: "r" (func));
        asm volatile("eret");
    } else {
        current->state = RUN_IN_USER_MODE;
        unsigned long user_stack = current->user_context.sp_el0;
        unsigned long user_cpu_state = current->user_context.spsr_el1;
        unsigned long user_pc = current->user_context.elr_el1;
        asm volatile("msr sp_el0, %0" :: "r" (user_stack));
        asm volatile("msr spsr_el1, %0" :: "r" (user_cpu_state));
        asm volatile("msr elr_el1, %0" :: "r" (user_pc));
        asm volatile("eret");
    }
}

void foo() // in el0
{
    unsigned long foo_count = 0;
    while(1) {
        uart_puts("foo: ");
        uart_hex(foo_count++);
        uart_puts("\n");
        // syscall - get task_id
        // asm volatile("mov x8, #1\n" "svc #0\n");        
        wait_cycles(10000000);
    }
}

// void test() {
//     int cnt = 1;
//     if (fork() == 0) {
//         fork();
//         delay(100000);
//         fork();
//         while(cnt < 10) {
//             printf("Task id: %d, cnt: %d\n", get_taskid(), cnt);
//             delay(100000);
//             ++cnt;
//         }
//         exit(0);
//         printf("Should not be printed\n");
//     } else {
//         printf("Task %d before exec, cnt address 0x%x, cnt value %d\n", get_taskid(), &cnt, cnt);
//         exec(foo);
//     }
// }