#include "sched.h"
#include "queue.h"
#include "uart.h"

#define COUNT_NUM 3

extern void  switch_to();

// struct task init = INIT_TASK;

void init_task(){
    // task_pool[0] = INIT_TASK;
    runqueue.head = 0;
    runqueue.tail = 0;
    task_pool[0].taskid = 0;
    asm volatile("msr tpidr_el1, %0"::"r"(&task_pool[0]));
}

void privilege_task_create(void(*func)()){
    static int task_id = 1;
    struct task *p;
    p = &task_pool[task_id];
    p -> cpu_context.sp = (unsigned long)kstack_pool[task_id]; //grow downward!!
    p -> cpu_context.lr = (unsigned long)func;
    p -> taskid = task_id++;
    p -> counter = COUNT_NUM;
    ENQUEUE(runqueue, MAX_TASK_NUM, p);
}

void context_switch(struct task* next){
  struct task* prev = current;
  switch_to(prev, next);
}

void _schedule(){
    struct task *p;
    DEQUEUE(runqueue, MAX_TASK_NUM, p);
    ENQUEUE(runqueue, MAX_TASK_NUM, p);
    context_switch(p);
}

void idle_schedule(){
    current->counter = 0;
    _schedule();
}

void schedule(){
    if (current->counter <= 0){
        current->counter = COUNT_NUM;
        uart_puts("reschedule flag set\n");
        _schedule();
    }
}