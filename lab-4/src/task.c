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
extern void fork_child_exit();

void task_manager_init(void(*func)()) {
    taskManager.taskCount = 0;
    taskManager.runningTaskId = 0;
    for(int i = 0; i < 64; i++) {
        taskManager.taskPool[i].state = ZOMBIE;
    }
    privilege_task_create(func);
    Task *initTask = &taskManager.taskPool[0];
    set_current(initTask);
    // uart_puts("user stack start: ");
    // uart_print_hex(&taskManager.ustackPool);
    // uart_puts("\n");
    // uart_print_hex(&taskManager.ustackPool[1]);
    // uart_puts("\n");
    // uart_print_hex(&taskManager.ustackPool[2]);
    // uart_puts("\n");
    // uart_puts("kernel stack start: ");
    // uart_print_hex(&taskManager.kstackPool);
    // uart_puts("\n");
    // uart_print_hex(&taskManager.kstackPool[1]);
    // uart_puts("\n");
    // uart_print_hex(&taskManager.kstackPool[2]);
    // uart_puts("\n");
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
    taskManager.taskPool[taskId].timeCount = 0;
    taskManager.taskCount++;
}

void context_switch(Task* next)
{
    Task *prev = get_current();
    uart_puts("prev taskid: ");
    uart_print_int(prev->id);
    uart_puts(", next taskid: ");
    uart_print_int(next->id);
    uart_puts("\n");
    // for(int i= 0; i<taskManager.taskCount; i++) {
    //     print_task_context(&taskManager.taskPool[i]);
    // }
    switch_to(prev, next);
}

void schedule() 
{
    Task *current = get_current();
    int nextTaskId = (current->id + 1) % 64;
    while(nextTaskId != current->id) {
        if (taskManager.taskPool[nextTaskId].state != ZOMBIE) {
            taskManager.runningTaskId = nextTaskId;
            context_switch(&taskManager.taskPool[nextTaskId]);
            return;
        }
        nextTaskId = (nextTaskId + 1) % taskManager.taskCount;
    }
}

void print_task_context(Task *task)
{
    uart_puts("Task id = ");
    uart_print_int(task->id);
    uart_puts(", sp_el0 = ");
    uart_print_hex(task->userContext.sp_el0);
    uart_puts(", elr_el1 = ");
    uart_print_hex(task->userContext.elr_el1);
    uart_puts(", spsr_el1 = ");
    uart_print_hex(task->userContext.spsr_el1);
    uart_puts(", sp = ");
    uart_print_hex(task->cpuContext.sp);
    uart_puts("\n");
}

void __exit(int status)
{
    Task *task = get_current();
    task->state = ZOMBIE;
    taskManager.taskCount --;
    schedule();
}

int __fork()
{
    int childTaskId;
    for(int i = 0; i < 64; i++) {
        if (taskManager.taskPool[i].state == ZOMBIE) {
            childTaskId = i;
            break;
        }
    }
    Task *parent = get_current();
    Task *child = &taskManager.taskPool[childTaskId];
    child->id = childTaskId;
    child->parentId = parent->id;
    memcpy(taskManager.kstackPool[parent->id], taskManager.kstackPool[childTaskId], 4096);
    memcpy(taskManager.ustackPool[parent->id], taskManager.ustackPool[childTaskId], 4096);

    child->cpuContext.x19 = parent->cpuContext.x19;
    child->cpuContext.x20 = parent->cpuContext.x20;
    child->cpuContext.x21 = parent->cpuContext.x21;
    child->cpuContext.x22 = parent->cpuContext.x22;
    child->cpuContext.x23 = parent->cpuContext.x23;
    child->cpuContext.x24 = parent->cpuContext.x24;
    child->cpuContext.x25 = parent->cpuContext.x25;
    child->cpuContext.x26 = parent->cpuContext.x26;
    child->cpuContext.x27 = parent->cpuContext.x27;
    child->cpuContext.x28 = parent->cpuContext.x28;

    Trapframe *trapframe = parent->trapframe;
    unsigned long sp_el0_offset = ((unsigned long) &taskManager.ustackPool[parent->id])
                                - (parent->userContext.sp_el0);
    unsigned long fp_offset = ((unsigned long) &taskManager.ustackPool[parent->id])
                                - trapframe->regs[29];
    unsigned long trapframe_offset = ((unsigned long) &taskManager.kstackPool[parent->id]) 
                                - parent->trapframe;


    child->trapframe = (unsigned long) &taskManager.kstackPool[child->id] - trapframe_offset;

    child->userContext.sp_el0 = (unsigned long) &taskManager.ustackPool[child->id] - sp_el0_offset;
    child->userContext.elr_el1 = parent->userContext.elr_el1;
    child->userContext.spsr_el1 = parent->userContext.spsr_el1;
    child->cpuContext.fp = (unsigned long) &taskManager.ustackPool[child->id] - fp_offset;
    child->cpuContext.sp = (unsigned long) &taskManager.kstackPool[child->id] - trapframe_offset;
    child->cpuContext.pc = (unsigned long) fork_child_exit;

    child->rescheduleFlag = 0;
    child->state = IN_KERNEL_MODE;
    child->timeCount = 0;
    taskManager.taskCount ++;
    uart_puts("Task ");
    uart_print_int(parent->id);
    uart_puts(" called fork\n");
    // print_task_context(child);
    return child->id;
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
        if (taskManager.taskCount == 1) {
            break;
        }
        schedule();
        wait(1000000);
    }
    uart_puts("Test finished\n");
    while(1);
}

void test() {
    int cnt = 1;
    if (fork() == 0) {
        fork();
        wait(100000);
        fork();
        while(cnt < 10) {
            uart_puts("TaskId: ");
            uart_print_int(get_taskid());
            uart_puts(", cnt: ");
            uart_print_int(cnt);
            uart_puts(", taskCount: ");
            uart_print_int(taskManager.taskCount);
            uart_puts("\n");
            wait(10000000);
            ++cnt;
        }
        exit(0);
        uart_puts("Should not be printed\n");
    } else {
        uart_puts("Task ");
        uart_print_int(get_taskid());
        uart_puts(" before exec, cnt address 0x");
        uart_print_hex(&cnt);
        uart_puts(", cnt value ");
        uart_print_int(cnt);
        uart_puts("\n");
        // printf("Task %d before exec, cnt address 0x%x, cnt value %d\n", get_taskid(), &cnt, cnt);
        exec(foo);
    }
}

void user_test() {
    do_exec(test);
}

void do_exec(void(*func)())
{
    Task *task = get_current();
    // uart_puts("do_exec TaskId: ");
    // uart_print_int(task->id);
    // uart_puts("\n");
    unsigned long userStack = task->userContext.sp_el0;
    unsigned long userCpuState = 0x0;
    asm volatile("msr sp_el0, %0" :: "r" (userStack));
    asm volatile("msr spsr_el1, %0" :: "r" (userCpuState));
    asm volatile("msr elr_el1, %0" :: "r" (func));
    asm volatile("eret");
}