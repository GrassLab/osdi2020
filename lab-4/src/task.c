#include "task.h"
#include "time.h"
#include "uart.h"

TaskManager taskManager;
Task tasks[64];

extern Task* get_current();
extern void set_current();
extern void switch_to(struct task* prev, struct task* next);
extern void switch_exit();

void taskManagerInit() {
    taskManager.taskCount = 0;
}

void privilege_task_create(void(*func)())
{
    unsigned int taskId = taskManager.taskCount;
    Task* task = &taskManager.taskPool[taskId]; 
    task->context.x19 = (unsigned long) func;
    task->context.pc = (unsigned long) switch_exit;
    task->context.sp = (unsigned long) &taskManager.kstackPool[taskId];
    task->id = taskId;
    taskManager.taskCount++;
}

void context_switch(Task* next)
{
    Task *prev = get_current();
    switch_to(prev, next);
}

void schedule() 
{
    for(int i = 0; i < taskManager.taskCount; ++i) {
        struct task* next = &taskManager.taskPool[i];
        context_switch(next);
    }
}

void foo(){
    while(1) {
        Task *task = get_current();
        uart_puts("Task id: ");
        uart_print_int(task->id);
        uart_puts("\n");
        wait(1000000);
        schedule();
    }
}

void idle(){
    while(1) {
        schedule();
        wait(1000000);
    }
}