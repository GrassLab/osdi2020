#include "syscall.h"
#include "task.h"

int get_taskid()
{
    unsigned long * x0;
    asm volatile("mov x8, %0" : :"r"(SYSCALL_GET_TASK_ID));
    asm volatile("svc #0");
    asm volatile("mov %0, x0" : "=r"(x0) :);
    return x0;    
}

int __get_taskid()
{
    Task *task = get_current();
    return task->id;
}