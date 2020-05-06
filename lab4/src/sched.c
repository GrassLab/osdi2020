#include "sched.h"
#include "queue.h"
#include "uart.h"
#include "sys.h"

#define COUNT_NUM 3
#define CPU_CONTEXT_NUM 13
#define TASK_RUNNING 1
#define TASK_ZOMBIE -1

extern void  switch_to();
extern void  ret_from_child();

// struct task init = INIT_TASK;
static int task_num = 1;

void init_task(){
    // task_pool[0] = INIT_TASK;
    runqueue.head = 0;
    runqueue.tail = 0;
    task_pool[0].taskid = 0;
    asm volatile("msr tpidr_el1, %0"::"r"(&task_pool[0]));
}

int privilege_task_create(void(*func)()){
    static int task_id = 1;
    struct task *p;
    p = &task_pool[task_id];
    p -> cpu_context.sp = (unsigned long)kstack_pool[task_id]; //grow downward!!
    p -> cpu_context.lr = (unsigned long)func;
    p -> taskid = task_id++;
    p -> counter = COUNT_NUM;
    p -> state = TASK_RUNNING;
    ENQUEUE(runqueue, MAX_TASK_NUM, p);
    ++task_num;
    return p -> taskid;
}

void context_switch(struct task* next){
  struct task* prev = current;
  switch_to(prev, next);
}

void _schedule(){
    struct task *p;
    DEQUEUE(runqueue, MAX_TASK_NUM, p);
    if(task_num == 1)
        context_switch(&task_pool[0]);
    if(p -> state == TASK_RUNNING){
        context_switch(p);
    }
}

void idle_schedule(){
    current->counter = 0;
    _schedule();
}

void schedule(){
    if (current->counter <= 0){
        current->counter = COUNT_NUM;
        uart_puts("reschedule flag set\n");
        // enable_irq();
        ENQUEUE(runqueue, MAX_TASK_NUM, current);
        _schedule();
    }
}

void do_exec(void(*func)()){
    asm volatile ("mov  x21, %0;"
                  "msr	elr_el1, x21"
                  ::"r"(func));
    asm volatile ("mov  x21, #0;"
                  "msr	spsr_el1, x21");
    asm volatile ("mov  x21, %0;"
                  "msr	sp_el0, x21"
                  ::"r"(ustack_pool[current->taskid]));
    asm volatile ("mov  sp, %0;"
                  ::"r"(kstack_pool[current->taskid] - 16));
    asm volatile ("eret");
    
    // func();
}

void do_fork(){
    int child_id = privilege_task_create(ret_from_child);
    for(int i=1; i<=4096; ++i){ 
        *(kstack_pool[child_id]-i) = *(kstack_pool[current->taskid]-i);
        *(ustack_pool[child_id]-i) = *(ustack_pool[current->taskid]-i);
    }
    set_trap_ret(current, child_id, 0);
    set_trap_ret(&task_pool[child_id], 0, 0);

    // fp, sp_el0
    int offset;
    offset = get_trap_arg(29) - (unsigned long)kstack_pool[current->taskid];
    set_trap_ret(&task_pool[child_id], (unsigned long)kstack_pool[child_id]+offset, 29);
    offset = get_trap_arg(31) - (unsigned long)ustack_pool[current->taskid];
    set_trap_ret(&task_pool[child_id], (unsigned long)ustack_pool[child_id]+offset, 31);
}

void do_exit(){
    current -> state = TASK_ZOMBIE;
    --task_num;
    _schedule();
}

int num_runnable_tasks(){
    return task_num;
}