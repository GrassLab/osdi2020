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
extern void ret_to_user();

// struct task* init_task = INIT_TASK;

void task_manager_init(void(*func)())
{
    TaskManager.queue_bitmap = 0;
    TaskManager.zombie_bitmap = 0;
    TaskManager.task_num = 0;

    // idle task
    unsigned int task_id = privilege_task_create(func, 0);
    struct task* init_task = &TaskManager.task_pool[task_id];
    set_current(&(init_task));
    
}


int privilege_task_create(void(*func)(), int fork_flag)
{
    // assign the task id
    unsigned int task_id = TaskManager.task_num;

    struct task* new_task = &TaskManager.task_pool[task_id]; 
    new_task->task_id = task_id;
    // uart_hex((unsigned long) new_task);
    if (fork_flag == 0) {
        new_task->cpu_context.x19 = (unsigned long) func;
        new_task->cpu_context.x20 = task_id;

        new_task->user_context.sp_el0 = &TaskManager.ustack_pool[task_id];
        new_task->user_context.spsr_el1 = 0;
        new_task->user_context.elr_el1 = 0;
        new_task->trapframe = 0;
        
        // uart_hex((unsigned long) func);
        new_task->cpu_context.pc = (unsigned long)ret_from_fork;
        new_task->cpu_context.sp = (unsigned long) &TaskManager.kstack_pool[task_id];
    } else {
        struct task* current = get_current();
        new_task->parent_id = current->task_id;
        uart_memcpy(&current->cpu_context, &new_task->cpu_context, 8*13);
        uart_memcpy(&current->user_context, &new_task->user_context, 8*3);
        uart_memcpy(&TaskManager.ustack_pool[current->task_id], &TaskManager.ustack_pool[task_id], 4096);
        uart_memcpy(&TaskManager.kstack_pool[current->task_id], &TaskManager.kstack_pool[task_id], 16*17);
        
        uart_puts("kstack_pool: ");
        uart_hex(&TaskManager.kstack_pool[task_id]);
        uart_puts(" ,ustack_pool: ");
        uart_hex(&TaskManager.ustack_pool[task_id]);
        uart_puts("\n");

        struct pt_regs* kstack_regs = TaskManager.kstack_pool[task_id];
        // *TaskManager.kstack_pool[task_id] = (unsigned long) 0x0;
        // *(&TaskManager.kstack_pool[task_id][8*31]) = &TaskManager.ustack_pool[task_id];
        // kstack_regs->regs[0] = 0;
        // kstack_regs->sp = &TaskManager.ustack_pool[task_id];


        new_task->user_context.sp_el0 = &TaskManager.ustack_pool[task_id];
        new_task->user_context.spsr_el1 = current->user_context.spsr_el1;
        new_task->user_context.elr_el1 = current->user_context.elr_el1;
        unsigned long trapframe_offset = (unsigned long) &TaskManager.kstack_pool[current->task_id] - current->trapframe;
        uart_puts("trapframe_offset: ");
        uart_hex(trapframe_offset);
        uart_puts("\n");

        // unsigned long pc;
        // asm volatile ("mrs %0, elr_el1" : "=r"(pc));
        new_task->cpu_context.x19 = (unsigned long) 0;
        new_task->cpu_context.pc = (unsigned long) ret_to_user;
        new_task->cpu_context.sp = (unsigned long) &TaskManager.kstack_pool[task_id] - trapframe_offset;
    }

    
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
    struct task* current = get_current();
    int next_task = (current->task_id+1) % TaskManager.task_num;
    struct task* next;
    while(1) {
        next = &TaskManager.task_pool[next_task];
        if (next->state != ZOMBIE) 
            break;
        next_task = (next_task+1) % TaskManager.task_num;
    } 
    next->counter = 3;
    uart_puts("switch to next task: ");
    uart_hex(next_task);
    uart_puts("\n");
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
 ** System call for do_exec , user context is replace by the provided one
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


int exit(int status)
{
    struct task* current = get_current();
    // release most of its resource but keepping the kernel stack and task struct
    // current->cpu_context = {0,0,0,0,0,0,0,0,0,0,0,0,0};
    // current->user_context = {0,0,0};
    uart_memset(current, 0, 8*16);
    // set its state to be zombie state and won’t be scheduled again.
    current->state = ZOMBIE;
    current->reschedule_flag = 1;
    TaskManager.zombie_bitmap &= (1<<current->task_id);
    schedule();
    return 0;
}

void zombie_reaper()
{
    struct task* check_task;
    for (int i=0; i<TaskManager.task_num; i++) {
        check_task = &TaskManager.task_pool[i];
        if (check_task->state == ZOMBIE) {
            uart_puts("I found a ZOMBIE in task: ");
            uart_hex(check_task->task_id);
            uart_puts("\n");
        } 
    }
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

void final_test_foo()
{
    int tmp = 5;
    // printf("Task %d after exec, tmp address 0x%x, tmp value %d\n", call_sys_get_taskid(), &tmp, tmp);
    unsigned long task_id = call_sys_get_taskid();
    uart_puts("Task ");
    uart_hex(task_id);
    uart_puts(" after exec, tmp address ");
    uart_hex(&tmp);
    uart_puts(", tmp value ");
    uart_hex(tmp);
    uart_puts("\n");

    call_sys_exit(0);
}


void final_test() 
{
    int cnt = 1;
    if (call_sys_fork() == 0) { // child
        uart_puts("I am child\n");
        call_sys_fork();
        wait_cycles(100000);
        call_sys_fork();
        while(cnt < 3) {
            // printf("Task id: %d, cnt: %d\n", call_sys_get_taskid(), cnt);
            unsigned long task_id = call_sys_get_taskid();
            uart_puts("Task id: ");
            uart_hex(task_id);
            uart_puts(", cnt: ");
            uart_hex(cnt);
            uart_puts("\n");

            wait_cycles(100000);
            ++cnt;
        }
        call_sys_exit(0);
        // printf("Should not be printed\n");
        uart_puts("Should not be printed\n");
    } else { // parent
        uart_puts("I am parent\n");
        // printf("Task %d before exec, cnt address 0x%x, cnt value %d\n", call_sys_get_taskid(), &cnt, cnt);
        unsigned long task_id = call_sys_get_taskid();
        uart_puts("Task ");
        uart_hex(task_id);
        uart_puts(" before exec, cnt address ");
        uart_hex(&cnt);
        uart_puts(", cnt value ");
        uart_hex(cnt);
        uart_puts("\n");

        call_sys_exec(final_test_foo);
    }
}

void final_user_test()
{
    do_exec(final_test);
}

void final_idle()
{
    while(1){
        if(TaskManager.task_num == 1) {
            break;
        }
        uart_puts("idle...\n");
        schedule();
        wait_cycles(1000000);
    }
    uart_puts("Test finished\n");
    while(1);
}