#include "sched.h"

#include "exce.h"
#include "mm.h"
#include "printf.h"
#include "timer.h"
#include "uart.h"

struct task_struct *current;              //currently executing task
struct task_struct *task[NR_TASKS]={0};   //task array
int nr_tasks = 0;                         //the number of currently running tasks

void proc_1()
{
    int n =100000000;
    uart_puts("process 1!!!\n");
    while (n--);
    schedule();
}

void proc_2()
{
    int n =100000000;
    uart_puts("process 2!!!\n");
    while (n--);
    schedule();
}

void proc_3()
{
    int n =100000000;
    uart_puts("process 3!!!\n");
    while (n--);
    schedule();
}

void idle_task()
{
    if (privilege_task_create((void *)&proc_1))
        uart_puts("error while starting process 1\n");
    if (privilege_task_create((void *)&proc_2))
        uart_puts("error while starting process 2\n");
    if (privilege_task_create((void *)&proc_3))
        uart_puts("error while starting process 3\n");
    core_timer_enable();

    while (1)
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
    asm ("ldp x29, x9, [x8], #16");
    asm ("ldr x30, [x8]");
    asm ("mov sp, x9");
}

void sched_init(void)
{
    int res = privilege_task_create(&idle_task);
    if (res != 0) {
        uart_puts("error while starting idle_task\n");
    }
    current = task[0];
    _load_contex(current);
}

void schedule(void)
{
    long idx = current->task_id;
    struct task_struct *next = task[(++idx)%NR_TASKS];
    while(!(next && next->state==TASK_RUNNING)){
        next = task[(++idx)%NR_TASKS];
    }
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
    asm ("stp x29, x9, [x8], #16");
    asm ("str x30, [x8]");
    asm ("add x8, x1, x10");
    asm ("ldp x19, x20, [x8], #16");
    asm ("ldp x21, x22, [x8], #16");
    asm ("ldp x23, x24, [x8], #16");
    asm ("ldp x25, x26, [x8], #16");
    asm ("ldp x27, x28, [x8], #16");
    asm ("ldp x29, x9, [x8], #16");
    asm ("ldr x30, [x8]");
    asm ("mov sp, x9");
	// msr tpidr_el1, x1         // store thread identifying information, for OS management purposes.
	return;						 // retruen to $ra($x30)
}

/*
 * save the current task’s context and restore the next task’s context.
 */
void context_switch(struct task_struct *next) 
{
    if (current == next) 
        return;
    struct task_struct *prev = current;
    current = next;
    _context_switch(prev, next);
}

/*
 * Initialize task struct and store in task_pool
 */
// TODO: Bad pid algorithm
int privilege_task_create(void(*func)())
{
    struct task_struct *pTask;
    if (!(pTask = (struct task_struct *) get_free_kstack()))
        return 1;
    pTask->state = TASK_RUNNING;
    pTask->cpu_context.x19 = (long)func;
    pTask->cpu_context.pc = (unsigned long)func;
    pTask->cpu_context.sp = (unsigned long)pTask + THREAD_SIZE;
    int task_id = nr_tasks++;
    pTask->task_id = task_id;
    task[task_id] = pTask;
    return 0;
}

void timer_tick()
{
    enable_irq();
    schedule();
    disable_irq();
}