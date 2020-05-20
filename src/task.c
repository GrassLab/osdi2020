#include "../include/uart.h"
#include "../include/time.h"
#include "../include/task.h"
#include "../include/queue.h"
#include "../include/syscall.h"
#include "../include/page.h"
#include "../include/mm.h"

struct task_manager TaskManager;

extern struct task* get_current();
extern void set_current(struct task* task_struct);

extern void switch_to(struct task* prev, struct task* next);
extern void ret_from_fork();
extern void ret_from_create();

extern void set_pgd(unsigned long pgd);
extern unsigned long get_pgd();

void task_manager_init(void(*func)())
{
    TaskManager.avail_bitmap = 0xffffffffffffffff;
    TaskManager.queue_bitmap = 0;
    TaskManager.zombie_num = 0;
    TaskManager.task_num = 0;

    // idle task
    unsigned int task_id = privilege_task_create(func, 0);
    // struct task* init_task = &TaskManager.task_pool[task_id];
    struct task* init_task = TaskManager.task_pool[task_id];
    set_current((struct task*)&init_task);
    
}
void stack_copy(void *src, void *dst, int len)
{
    char *s = src;
    char *d = dst;
    while (len--) {
        *d-- = *s--;
        //if (*s == 0) break;
    }
}

int privilege_task_create(void(*func)(), int fork_flag)
{
    // assign the task id
    unsigned int task_id = TaskManager.task_num;
    struct task* new_task = allocate_kernel_page();
    if (!new_task) 
        return -1;
    // struct task* new_task = &TaskManager.task_pool[task_id]; 
    TaskManager.task_pool[task_id] = new_task;
    printf("new_task at %x\n", new_task);
    new_task->task_id = task_id;

    struct trapframe_regs* kstack_regs = get_task_trapframe(new_task);
    memzero((unsigned long)kstack_regs, sizeof(struct trapframe_regs));
	memzero((unsigned long)&new_task->cpu_context, sizeof(struct cpu_context));
	memzero((unsigned long)&new_task->mm, sizeof(struct mm_struct));
    if (fork_flag == 0) {
        printf("privilege_task_create\n");
        new_task->cpu_context.x19 = (unsigned long) func;
        new_task->cpu_context.x20 = task_id;

        // new_task->user_context.sp_el0 = &TaskManager.ustack_pool[task_id];
        // new_task->user_context.spsr_el1 = 0;
        // new_task->user_context.elr_el1 = 0;
        
        new_task->cpu_context.fp = 0;
        new_task->cpu_context.pc = (unsigned long) ret_from_create;
        // new_task->cpu_context.sp = (unsigned long) &TaskManager.kstack_pool[task_id];
        new_task->cpu_context.sp = (unsigned long) kstack_regs;

    } else {
        struct task* current = get_current();
        new_task->parent_id = current->task_id;
        uart_memcpy(&current->cpu_context, &new_task->cpu_context, 8*10); // x19 - x28
        stack_copy(&TaskManager.ustack_pool[current->task_id], &TaskManager.ustack_pool[task_id], 2048);
        stack_copy(&TaskManager.kstack_pool[current->task_id], &TaskManager.kstack_pool[task_id], 2048);
        
        struct trapframe_regs* kstack_regs = current->trapframe;

        unsigned long ustack_offset = ((unsigned long) &TaskManager.ustack_pool[current->task_id]) - kstack_regs->sp_el0;
        unsigned long trapframe_offset = ((unsigned long) &TaskManager.kstack_pool[current->task_id]) - current->trapframe;
        unsigned long fp_offset = ((unsigned long) &TaskManager.ustack_pool[current->task_id]) - kstack_regs->regs[29];


        new_task->user_context.sp_el0 = (unsigned long) &TaskManager.ustack_pool[task_id] - ustack_offset;
        new_task->user_context.spsr_el1 = kstack_regs->spsr_el1;
        new_task->user_context.elr_el1 = kstack_regs->elr_el1;
        
        new_task->cpu_context.fp = (unsigned long) &TaskManager.ustack_pool[task_id] - fp_offset;
        new_task->cpu_context.pc = (unsigned long) ret_from_fork;
        new_task->cpu_context.sp = (unsigned long) &TaskManager.kstack_pool[task_id] - trapframe_offset;
    }
    
    new_task->trapframe = 0;
    new_task->reschedule_flag = 0;
    new_task->state = RUN_IN_KERNEL_MODE;
    TaskManager.task_num++;

    return task_id;
}

struct trapframe_regs* get_task_trapframe(struct task *task)
{
	 //a small area (pt_regs area) was reserved at the top of the stack of the newly created task.
	struct trapframe_regs* p = (unsigned long)task + 4096 - sizeof(struct trapframe_regs);
	return p;
}

void context_switch(struct task* next)
{
    struct task* current = get_current();
    struct task* prev = current;
    // disable_irq();
    printf("next->mm.pgd: %x\n", next->mm.pgd);
    set_pgd(next->mm.pgd);
    switch_to(prev, next);
}



void schedule()
{
    // uart_puts("\r\n++++++++++  scheduler begin  ++++++++++\n");
    struct task* current = get_current();
    int next_task = (current->task_id+1) % TaskManager.task_num;
    struct task* next;
    while(1) {
        next = TaskManager.task_pool[next_task];
        if (next->state != ZOMBIE) 
            break;
        next_task = (next_task+1) % TaskManager.task_num;
    } 
    if (next->state == ZOMBIE) {
        uart_puts("OH NO ZOMBIE\n");
    }
    next->counter = CNT;
    uart_puts("switch to next task: ");
    uart_hex(next_task);
    uart_puts(", state: ");
    uart_hex(next->state);
    uart_puts("\n");
    context_switch(next);

    // while (!QUEUE_EMPTY(RunQueue)) {
    //     struct task* next = QUEUE_GET(RunQueue);
    //     QUEUE_POP(RunQueue);

    //     QUEUE_SET(RunQueue, (unsigned long) next);
    //     QUEUE_PUSH(RunQueue);

    //     context_switch(next);
    // }
    // uart_puts("++++++++++  scheduler end  ++++++++++\n\r\n");
}

/*
 ** takes a function pointer to user code
 ** System call for do_exec , user context is replace by the provided one
 */
void do_exec(unsigned long start, unsigned long size, void(*func)())
{
    // be triggered at the first time execute
    struct task* current = get_current();

    if (current->reschedule_flag == 1) {
        uart_puts("kernel routine reschedule...\n");
        current->reschedule_flag = 0;
        schedule();
    }

    if (current->state == RUN_IN_KERNEL_MODE) { // first user task
        // return to user mode
        struct trapframe_regs* kstack_regs = get_task_trapframe(current);
        // memzero((unsigned long)kstack_regs, sizeof(struct trapframe_regs));

        kstack_regs->sp_el0 = 2 *  PAGE_SIZE;//0x0000ffffffffe000;
        kstack_regs->spsr_el1 = 0;//PSR_MODE_EL0t;
        kstack_regs->elr_el1 = func;

        unsigned long code_page = allocate_user_page(current, func);
        if (code_page == 0)	{
            return -1;
        }
        memcpy(code_page, start, size);
        // uart_puts("new user context\n");
        current->state = RUN_IN_USER_MODE;
        // unsigned long ttbr1_el1;
        // unsigned long ttbr0_el1;
        // asm volatile ("mrs %0, ttbr1_el1" : "=r"(ttbr1_el1));
        // asm volatile ("mrs %0, ttbr0_el1" : "=r"(ttbr0_el1));
        // printf("ttbr0_el1: %x\n", ttbr0_el1);
        // printf("ttbr1_el1: %x\n", ttbr1_el1);
        unsigned long user_pgd = current->mm.pgd;
        // printf("user_pgd: %x\n", user_pgd);
        set_pgd(user_pgd);
        return 0;
        
        // unsigned long user_stack = 0x0000ffffffffe000;//current->user_context.sp_el0;
        // unsigned long user_cpu_state = PSR_MODE_EL0t;
        // asm volatile("msr sp_el0, %0" :: "r" (user_stack));
        // asm volatile("msr spsr_el1, %0" :: "r" (user_cpu_state));
        // asm volatile("msr elr_el1, %0" :: "r" (func));
        // asm volatile("eret");

    } else if (current->state == EXC_CONTEXT){ // exeception handler
        struct trapframe_regs* kstack_regs = current->trapframe;
        kstack_regs->sp_el0 = TaskManager.ustack_pool[current->task_id]; 
        kstack_regs->elr_el1 = func;
        kstack_regs->spsr_el1 = 0;

    } else {
        uart_puts("do_exec() error");
        while(1);
    }
}


int do_fork() 
{
    int pid;
    pid = privilege_task_create(0, 1);
    return pid;
}


int do_exit(int status)
{
    struct task* current = get_current();
    // release most of its resource but keepping the kernel stack and task struct
    // current->cpu_context = {0,0,0,0,0,0,0,0,0,0,0,0,0};
    // current->user_context = {0,0,0};
    // uart_memset(current, 0, 8*16);
    // set its state to be zombie state and won’t be scheduled again.
    current->state = ZOMBIE;
    current->reschedule_flag = 1;
    // TaskManager.zombie_bitmap &= (1<<current->task_id);
    TaskManager.zombie_num++;
    zombie_reaper(current);
    schedule();
    return 0;
}

void zombie_reaper(struct task* check_task)
{
    TaskManager.avail_bitmap &= (1 << (check_task->task_id));
    // uart_puts("I kill a ZOMBIE task: ");
    // uart_hex(check_task->task_id);
    // uart_puts("\n");
}


/* 
 ** kernel porcess for test
 */
// void kernel_test(unsigned int task_id)
// {
//     while(1) {
//         struct task* current = get_current();
//         uart_puts("Task_id: ");
//         // uart_hex(current->task_id);
//         uart_hex(task_id);
//         uart_puts("\n");
            
//         // delay(1000000);
//         wait_cycles(1000000);
//         if (current->reschedule_flag == 1) {
//             uart_puts("reschedule...\n");
//             current->reschedule_flag = 0;
//             schedule();
//         }
//     }
// }

void idle()
{
    while(1){
        uart_puts("idle...\n");
        schedule();
        // delay(1000000);
        wait_cycles(1000000);
    }
}

// /* 
//  ** user porcess for test
//  */
// void user_test()
// {
//     do_exec(foo);
// }

// void hello()
// {
//     char read_char;
//     while(1) {
//         read_char = call_sys_uart_read(); // it will wait but still can be interrupt
//         call_sys_uart_write("hello world in foo\n");

//         wait_cycles(1000000);
//     }

// }
// void foo() // in el0
// {
//     unsigned long foo_count = 0;
//     unsigned long sys_ret_val;
//     // exec(hello);
//     uart_puts("foo: ");
//     uart_hex(foo_count++);
//     uart_puts("\n");
//     // syscall - get task_id
//     // sys_ret_val = call_sys_get_taskid();
//     // uart_puts("sys_ret_val: ");
//     // uart_hex(sys_ret_val); 
//     // uart_puts("\n");     
//     wait_cycles(10000000);
//     while(1);
// }

// void final_test_foo()
// {
//     int tmp = 5;
//     // printf("Task %d after exec, tmp address 0x%x, tmp value %d\n", call_sys_get_taskid(), &tmp, tmp);
//     unsigned long task_id = call_sys_get_taskid();
//     uart_puts("Task ");
//     uart_hex(task_id);
//     uart_puts(" after exec, tmp address ");
//     uart_hex(&tmp);
//     uart_puts(", tmp value ");
//     uart_hex(tmp);
//     uart_puts("\n");

//     exit(0);
// }


// void final_test() 
// {
//     int cnt = 1;
//     if (fork() == 0) { // child
//         uart_puts("I am child\n");
//         fork();
//         wait_cycles(1000000);
//         fork();
//         unsigned long task_id = call_sys_get_taskid();
//         uart_puts("#### Task id: ");
//         uart_hex(task_id);
//         uart_puts(", cnt: ");
//         uart_hex(cnt);
//         uart_puts(", &cnt: ");
//         uart_hex(&cnt);
//         uart_puts(" ####\n");
//         while(cnt < 10) {
//             // printf("Task id: %d, cnt: %d\n", call_sys_get_taskid(), cnt);
//             unsigned long task_id = call_sys_get_taskid();
//             uart_puts("Task id: ");
//             uart_hex(task_id);
//             uart_puts(", cnt: ");
//             uart_hex(cnt);
//             uart_puts(", &cnt: ");
//             uart_hex(&cnt);
//             uart_puts("\n");

//             wait_cycles(1000000);
//             ++cnt;
//         }
//         exit(0);
//         // printf("Should not be printed\n");
//         uart_puts("Should not be printed\n");
//     } else { // parent
//         uart_puts("I am parent\n");
//         // printf("Task %d before exec, cnt address 0x%x, cnt value %d\n", call_sys_get_taskid(), &cnt, cnt);
//         unsigned long task_id = call_sys_get_taskid();
//         uart_puts("Task ");
//         uart_hex(task_id);
//         uart_puts(" before exec, cnt address ");
//         uart_hex(&cnt);
//         uart_puts(", cnt value ");
//         uart_hex(cnt);
//         uart_puts("\n");
//         cnt = 5;
//         exec(final_test_foo);
//     }
// }

// void final_user_test()
// {
//     do_exec(final_test);
// }

// void final_idle()
// {
//     while(1){
//         // uart_hex(TaskManager.task_num);
//         // uart_hex(TaskManager.zombie_num);
//         uart_puts("idle...\n");
//         if(TaskManager.task_num == TaskManager.zombie_num+1) {
//             break;
//         }
//         schedule();
//         wait_cycles(1000000);
//     }
//     uart_puts("Test finished\n");
//     disable_irq();
//     while(1);
// }