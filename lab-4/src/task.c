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
    if (func == 0) { // fork
        return;
    }

    int taskId = taskManager.taskCount;
    Task* task = &taskManager.taskPool[taskId];
    task->cpuContext.pc = (unsigned long) func;
    task->cpuContext.sp = (unsigned long) &taskManager.kstackPool[taskId];
    task->userContext.sp_el0 = &taskManager.ustackPool[taskId];
    task->userContext.spsr_el1 = 0;
    task->userContext.elr_el1 = 0;
    task->id = taskId;
    task->parentId = 0;
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
    Task *current = get_current();
    int nextTaskId = (current->id + 1) % taskManager.taskCount;
    while(nextTaskId != current->id) {
        if (taskManager.taskPool[nextTaskId].state != ZOMBIE) {
            taskManager.runningTaskId = nextTaskId;
            context_switch(&taskManager.taskPool[nextTaskId]);
            return;
        }
        nextTaskId = (nextTaskId + 1) % taskManager.taskCount;
    }
}

void __exit(int status)
{
    Task *task = get_current();
    task->state = ZOMBIE;
    schedule();
}

int fork()
{
    int childTaskId = taskManager.taskCount;
    Task *parent = get_current();
    Task *child = &taskManager.taskPool[childTaskId];
    // uart_puts("parent: ");
    // uart_print_int(parent->id);
    // uart_puts("\n");
    child->parentId = 0;
    memcpy(taskManager.kstackPool[parent->id], taskManager.kstackPool[childTaskId], 4096);
    memcpy(taskManager.ustackPool[parent->id], taskManager.kstackPool[childTaskId], 4096);
    // child->userContext.sp_el0 = ((unsigned long) &taskManager.ustackPool[parent->id]) - ;
}

void foo()
{
    while(1) {
        uart_puts("TaskId: ");
        uart_print_int(get_taskid());
        uart_puts("\n");
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

void test() {
    exec(foo);
    exit(0);
}

void user_test() {
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