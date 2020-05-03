#include "task.h"
#include "time.h"
#include "uart.h"
#include "syscall.h"

TaskManager taskManager;
Task tasks[64];

extern Task* get_current();
extern void set_current();
extern void switch_to(struct task* prev, struct task* next);
extern void switch_exit();

void task_manager_init() {
    taskManager.taskCount = 0;
}

void privilege_task_create(void(*func)())
{
    unsigned int taskId = taskManager.taskCount;
    Task* task = &taskManager.taskPool[taskId]; 
    task->cpuContext.x19 = (unsigned long) func;
    task->cpuContext.pc = (unsigned long) switch_exit;
    task->cpuContext.sp = (unsigned long) &taskManager.kstackPool[taskId];
    task->userContext.sp_el0 = &taskManager.ustackPool[taskId];
    task->userContext.spsr_el1 = 0;
    task->userContext.elr_el1 = 0;
    task->id = taskId;
    task->rescheduleFlag = 0;
    task->state = IN_KERNEL_MODE;
    taskManager.runningTaskId = 0;
    taskManager.taskPool[taskId].timeCount = 0;
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
        Task *next = &taskManager.taskPool[i];
        taskManager.runningTaskId = next->id;
        context_switch(next);
    }
}

void foo()
{
    while(1) {
        get_taskid();
        wait(100000000);
    }
}

void idle()
{
    while(1) {
        schedule();
        wait(1000000);
    }
}

void user_task_test() {
    do_exec(foo);
}

void do_exec(void(*func)())
{
    Task *task = get_current();
    unsigned long userStack = task->userContext.sp_el0;
    unsigned long userCpuState = 0x0;
    asm volatile("msr sp_el0, %0" :: "r" (userStack));
    asm volatile("msr spsr_el1, %0" :: "r" (userCpuState));
    asm volatile("msr elr_el1, %0" :: "r" (func));
    asm volatile("eret");
}