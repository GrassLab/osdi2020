#include "../include/uart.h"
#include "../include/time.h"
#include "../include/task.h"
#include "../include/queue.h"
#include "../include/syscall.h"


// #define INIT_TASK \
// { \
// /* cpu_context */   {0,0,0,0,0,0,0,0,0,0,0,0,0}, \
// /* user_context */   {0,0,0}, \
// /* state etc */	    0,0,0,0,0,0 \
// }

struct task_manager TaskManager;

extern struct task* get_current();
extern void set_current(struct task* task_struct);

extern void switch_to(struct task* prev, struct task* next);
extern void ret_from_fork();
extern void ret_from_exec();

// struct task* init_task = INIT_TASK;

void task_manager_init()
{
    TaskManager.queue_bitmap = 0;
    TaskManager.task_num = 0;
    TaskManager.next_task = 0;

    // idle task
    unsigned int task_id = privilege_task_create(idle, 0);
    struct task* init_task = &TaskManager.task_pool[task_id];
    set_current(&(init_task));
    
}

int privilege_task_create(void(*func)(), int fork_flag)
{
    // assign the task id
    unsigned int task_id = TaskManager.task_num;

    struct task* new_task = &TaskManager.task_pool[task_id]; 
    // uart_hex((unsigned long) new_task);
    if (fork_flag == 0) {
        new_task->cpu_context.x19 = (unsigned long) func;
        new_task->cpu_context.x20 = task_id;
    } else {
        struct task* current = get_current();
        new_task->cpu_context.x19 = current->cpu_context.x19;
        new_task->cpu_context.x20 = current->cpu_context.x20;
        new_task->parent_id = current->task_id;
    }
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
    return task_id;
}

void context_switch(struct task* next)
{
    struct task* current = get_current();
    struct task* prev = current;
    switch_to(prev, next);
}



void schedule()
{
    uart_puts("\r\n++++++++++  scheduler begin  ++++++++++\n");
    uart_puts("switch to next task: ");
    uart_hex(TaskManager.next_task);
    uart_puts("\n");
    struct task* next = &TaskManager.task_pool[TaskManager.next_task];
    next->counter = 3;
    TaskManager.next_task = (TaskManager.next_task+1) % TaskManager.task_num;
    context_switch(next);

    // while (!QUEUE_EMPTY(RunQueue)) {
    //     struct task* next = QUEUE_GET(RunQueue);
    //     QUEUE_POP(RunQueue);

    //     QUEUE_SET(RunQueue, (unsigned long) next);
    //     QUEUE_PUSH(RunQueue);

    //     context_switch(next);
    // }
    uart_puts("++++++++++  scheduler end  ++++++++++\n\r\n");
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
        uart_puts("new user context\n");
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


int fork() 
{
    struct task* current = get_current();
    int pid;
    pid = privilege_task_create(0, 1);
    return pid;
}

void hello()
{
    while(1) {
        uart_puts("hello world in foo\n");
        wait_cycles(10000000);
    }

}
void foo() // in el0
{
    unsigned long foo_count = 0;
    unsigned long sys_ret_val;
    call_sys_exec(hello);
    while(1) {
        uart_puts("foo: ");
        uart_hex(foo_count++);
        uart_puts("\n");
        // syscall - get task_id
        sys_ret_val = call_sys_get_taskid();
        // asm volatile("mov x8, #1\n" "svc #0\n"); 
        // asm volatile("mov %0, x0" : "=r"(sys_ret_val));

        uart_puts("sys_ret_val: ");
        uart_hex(sys_ret_val); 
        uart_puts("\n");     
        wait_cycles(10000000);
    }
}

void test_foo()
{
    int tmp = 5;
    // printf("Task %d after exec, tmp address 0x%x, tmp value %d\n", call_sys_get_taskid(), &tmp, tmp);
    call_sys_exit(0);
}


void test() 
{
    int cnt = 1;
    if (call_sys_fork() == 0) {
        call_sys_fork();
        wait_cycles(100000);
        call_sys_fork();
        while(cnt < 10) {
            // printf("Task id: %d, cnt: %d\n", call_sys_get_taskid(), cnt);
            wait_cycles(100000);
            ++cnt;
        }
        call_sys_exit(0);
        // printf("Should not be printed\n");
    } else {
        // printf("Task %d before exec, cnt address 0x%x, cnt value %d\n", call_sys_get_taskid(), &cnt, cnt);
        call_sys_exec(test_foo);
    }
}