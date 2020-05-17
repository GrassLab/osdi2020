#include "schedule.h"

#include "exce.h"
#include "mm.h"
#include "printf.h"
#include "timer.h"
#include "uart.h"
#include "svc.h"
#include "sysregs.h"
#include "string.h"

void delay(int cnt)
{
    while(cnt--);
}

struct task_struct *current;                   //currently executing task
struct task_struct *task_pool[NR_TASKS]={0};   //task array

void foo(){// user code
    int tmp = 5;
    printf("Task %d after  exec, tmp address 0x%x, tmp value %d\n", sys_get_taskid(), &tmp, tmp);
    sys_exit(0);
}

/***Unknown reason refer to same cnt***/
void test() {//user code
    int cnt = 1;
    int pid;
    if ((pid = sys_fork()) == 0) {
        sys_fork();
        delay(100000);
        sys_fork();
        while(cnt < 10) {
            printf("Task id: %d, cnt: %d\n", sys_get_taskid(), cnt);
            delay(100000);
            ++cnt;
        }
        sys_exit(0);
    printf("Should not be printed\n");
    } else {
        printf("Task %d before exec, cnt address 0x%x, cnt value %d\n", sys_get_taskid(), &cnt, cnt);
        wait(pid);
        sys_exec(foo);
  }
}

void user_test(){//kernel code
    do_exec(test);
}

void startup_task()
{
    // for(int i = 0; i < 5; ++i) { // N should > 2
    //     privilege_task_create(foo);
    // }
    privilege_task_create(user_test);
    core_timer_enable();

    while (1) // idle here
        schedule();
}

void _load_contex(){
    asm ("mov x10, #0");
    asm ("add x8, x0, x10");
    asm ("ldp x19, x20, [x8], #16");
    asm ("ldp x21, x22, [x8], #16");
    asm ("ldp x23, x24, [x8], #16");
    asm ("ldp x25, x26, [x8], #16");
    asm ("ldp x27, x28, [x8], #16");
    asm ("ldp x29, x30, [x8], #16");
    asm ("ldr x9, [x8]");
    asm ("mov sp, x9");
}

void sched_init(void)
{
    int res = privilege_task_create(&startup_task);
    if (res != 0) {
        uart_puts("error while starting idle_task\n");
    }
    current = task_pool[0];
    _load_contex(current);
    uart_puts("[sched_init] should not got here!\n");
}

void schedule(void)
{
    long idx = current->task_id;
    struct task_struct *next = task_pool[(++idx)%NR_TASKS];
    while(!(next && next->state==TASK_RUNNING))
        next = task_pool[(++idx)%NR_TASKS];
    context_switch(next);
}

void _context_switch()
{
    asm ("mov x10, #0");
    asm ("add x8, x0, x10");
    asm ("mov x9, sp");
    asm ("stp x19, x20, [x8], #16");
    asm ("stp x21, x22, [x8], #16");
    asm ("stp x23, x24, [x8], #16");
    asm ("stp x25, x26, [x8], #16");
    asm ("stp x27, x28, [x8], #16");
    asm ("stp x29, x30, [x8], #16"); //fp,lr
    asm ("str x9, [x8]");//sp
    asm ("add x8, x1, x10");
    asm ("ldp x19, x20, [x8], #16");
    asm ("ldp x21, x22, [x8], #16");
    asm ("ldp x23, x24, [x8], #16");
    asm ("ldp x25, x26, [x8], #16");
    asm ("ldp x27, x28, [x8], #16");
    asm ("ldp x29, x30, [x8], #16");
    asm ("ldr x9, [x8]");
    asm ("mov sp, x9");
	// msr tpidr_el1, x1         // store thread identifying information, for OS management purposes.
	return;						 // retruen to $lr($x30)
}

void context_switch(struct task_struct *next) 
{
    if (current == next) 
        return;
    struct task_struct *prev = current;
    current = next;
    _context_switch(prev, next);
}

static long assign_task_id()
{
    for(long id=0; id<64; id++){
        if(task_pool[id]) // non Null ptr, if assigned.
            continue;
        return id;
    }
    uart_puts("Task pool had fulled!");
    return -1;
}

//Our sp offset is base on pTask. It only works on fix stack location.
int privilege_task_create(void(*func)())
{
    struct task_struct *pTask;
    long task_id = assign_task_id();
    pTask = (struct task_struct *)get_kstack_base(task_id);
    pTask->state = TASK_RUNNING;
    // pTask->cpu_context.x19 = (long)func;
    unsigned long sp_offset = THREAD_SIZE;
    pTask->cpu_context.sp = (unsigned long)pTask + sp_offset;
    // pTask->cpu_context.fp = 
    pTask->cpu_context.lr = (unsigned long)func;
    pTask->counter = 4;
    pTask->schedule_flag = 0;
    pTask->task_id = task_id;
    task_pool[task_id] = pTask;
    return task_id;
}

void wait(long task_id){
    while (1)
    {
        if(task_pool[task_id]->state == TASK_ZOMBIE){
            task_pool[task_id] = 0;
            break;
        }
        sys_sched_yield();
    }
    
}

// memcpy need overwrite old_text?
void do_exec(void(*func)())
{
    unsigned long usp = (unsigned long)current + THREAD_SIZE + STACK_OFFSET;
    asm volatile("msr sp_el0, %0"::"r"(usp):);
    asm volatile("msr spsr_el1, %0"::"r"(SPSR_EL1_VALUE):);
    asm volatile("msr elr_el1, %0"::"r"(func):);
    asm volatile("eret");
}

void do_fork(struct trapframe *tf)
{
    struct task_struct *pTask;
    long task_id = assign_task_id();
    pTask = (struct task_struct *) get_kstack_base(task_id);
    task_pool[task_id] = pTask;

    memcpy(pTask, current, THREAD_SIZE);//parent stack
    memcpy(pTask + STACK_OFFSET, current + STACK_OFFSET, THREAD_SIZE);//user stack

    pTask->task_id = task_id;
    // set ksp
    unsigned long tf_offset = (unsigned long)tf - (unsigned long)current;
    pTask->cpu_context.sp = (unsigned long)pTask + tf_offset;
    // set usp
    struct trapframe *utf = (struct trapframe *)pTask->cpu_context.sp;
    unsigned long usp_offset = tf->sp_el0 - (unsigned long)current;
    utf->sp_el0 = (unsigned long)pTask + usp_offset;
    // set fp
    // Note: complier will use fp to access function local variable. not sp
    unsigned long fp_offset = (unsigned long)tf->fp - (unsigned long)current;
    utf->fp = (unsigned long)pTask + fp_offset;
    // set return
    tf->Xn[0] = task_id;
    utf->Xn[0] = 0;
    // need not set cpu context, kernel exit will overwrite it.
    pTask->cpu_context.lr = (unsigned long)ret_fork_child;
}

void do_exit(int status)
{
    current->state = TASK_ZOMBIE;
    current->schedule_flag = 1;
    // should call schedule later by task preemption
}

void timer_tick()
{
    enable_irq();
    if(current->counter > 0){
        current->counter--;
        printf("  Task_id:%d  Counter value: %d\n",
                 current->task_id, current->counter);
    }else
        current->schedule_flag=1;
    disable_irq();
}

void task_preemption()
{
    if(current->schedule_flag)
        schedule();
}