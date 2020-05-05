#include "task.h"
#include "time.h"
#include "uart.h"
#include "syscall.h"

TaskManager taskManager;
Task tasks[64];

extern Task* get_current();
extern void set_current();
extern void switch_to(Task* prev, Task* next);
extern void switch_exit();

void task_manager_init() {
    taskManager.taskCount = 0;
    uart_puts("user stack start: ");
    uart_print_hex(&taskManager.ustackPool);
    uart_puts("\n");
    uart_print_hex(&taskManager.ustackPool[1]);
    uart_puts("\n");
    uart_print_hex(&taskManager.ustackPool[2]);
    uart_puts("\n");
    uart_puts("kernel stack start: ");
    uart_print_hex(&taskManager.kstackPool);
    uart_puts("\n");
    uart_print_hex(&taskManager.kstackPool[1]);
    uart_puts("\n");
    uart_print_hex(&taskManager.kstackPool[2]);
    uart_puts("\n");
}

void privilege_task_create(void(*func)())
{
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

int __fork()
{
    // int childTaskId = taskManager.taskCount; // should find child id
    // Task *parent = get_current();
    // Task *child = &taskManager.taskPool[childTaskId];
    // child->parentId = parent->id;
    // memcpy(taskManager.kstackPool[parent->id], taskManager.kstackPool[childTaskId], 4096);
    // memcpy(taskManager.ustackPool[parent->id], taskManager.kstackPool[childTaskId], 4096);

    // unsigned long sp_el0_offset = ((unsigned long) &taskManager.ustackPool[parent->id])
    //                             - (parent->userContext.sp_el0);
    // child->userContext.sp_el0 = (unsigned long) &taskManager.ustackPool[child->id] - sp_el0_offset;

    // unsigned long fp_offset = ((unsigned long) &taskManager.ustackPool[parent->id]) - kstack_regs->regs[29];
}

void foo()
{
    int count = 0;
    while(1) {
        uart_puts("TaskId: ");
        uart_print_int(get_taskid());
        uart_puts(", Count: ");
        uart_print_int(count++);
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