#include "schedule.h"

#include "exce.h"
#include "mm.h"
#include "printf.h"
#include "timer.h"
#include "uart.h"
#include "sysregs.h"

struct task_struct *current;                   //currently executing task
struct task_struct *task_pool[NR_TASKS]={0};   //task array
int nr_tasks = 0;                              //the number of currently running tasks


void foo(){
  while(1) {
    printf("Task id: %d\n", current->task_id);
    int n = 10000000;
    while(n--);
    schedule();
  }
}

void test() {
    while (1)
    {
        int cnt = 1;
        int n = 10000000;
        while(n--);
        uart_puts("EL0 task\n");
    }
//   if (fork() == 0) {
//     fork();
//     delay(100000);
//     fork();
//     while(cnt < 10) {
//       printf("Task id: %d, cnt: %d\n", get_taskid(), cnt);
//       delay(100000);
//       ++cnt;
//     }
//     exit(0);
//     printf("Should not be printed\n");
//   } else {
//     printf("Task %d before exec, cnt address 0x%x, cnt value %d\n", get_taskid(), &cnt, cnt);
//     exec(foo);
//   }
}

void user_test(){
  do_exec(test);
}

void startup_task()
{
    for(int i = 0; i < 5; ++i) { // N should > 2
        privilege_task_create(foo);
    }
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
    asm ("ldp x29, x9, [x8], #16");
    asm ("ldr x30, [x8]");
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

void context_switch(struct task_struct *next) 
{
    if (current == next) 
        return;
    struct task_struct *prev = current;
    current = next;
    _context_switch(prev, next);
}

long _assign_task_id()
{
    for(long id=0; id<64; id++){
        if(task_pool[id]) // non Null ptr, if assigned.
            continue;
        return id;
    }
    uart_puts("Task pool had fulled!");
    return -1;
}

int privilege_task_create(void(*func)())
{
    struct task_struct *pTask;
    long task_id = _assign_task_id();
    pTask = (struct task_struct *) get_kstack_base(task_id);
    pTask->state = TASK_RUNNING;
    pTask->cpu_context.x19 = (long)func;
    pTask->cpu_context.pc = (unsigned long)func;
    pTask->cpu_context.sp = (unsigned long)pTask + THREAD_SIZE;
    pTask->user_context.SP_EL0 = pTask->cpu_context.sp + STACK_OFFSET;
    pTask->counter = 4;
    pTask->schedule_flag = 0;
    pTask->task_id = task_id;
    task_pool[task_id] = pTask;
    nr_tasks++;
    return 0;
}

void do_exec(void(*func)())
{
	asm volatile("msr sp_el0, %0"::"r"(current->user_context.SP_EL0):);
	asm volatile("msr spsr_el1, %0"::"r"(SPSR_EL1_VALUE):);
	asm volatile("msr elr_el1, %0"::"r"(func):);
	asm volatile("eret");
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