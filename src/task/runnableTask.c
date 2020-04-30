#include "type.h"
#include "device/uart.h"
#include "task/taskManager.h"
#include "task/schedule.h"
#include "task/sysCall.h"

// User task

void userTask()
{
    asm volatile("svc #2"); // Enable timer

    uartPuts("user task\n");
}

void forkTask()
{
    asm volatile("svc #2"); // Enable timer

    uartPuts("user task\n");
    // int r = fork();
    // uartInt(r);
    if (fork() == 0)
    {
        uartPuts("parent\n");
    }
    else
    {
        uartPuts("child\n");
    }
}

// Kernel task

void execTask()
{
    doExec(&forkTask);
}

void idleTask()
{
    while (1)
    {
        schedule();

        for (int i = 0; i < 1000000; ++i)
        {
            asm volatile("nop");
        }
    }
}

void kernelTask()
{
    while (1)
    {
        if (current->re_schedule == true)
        {
            uartPuts("Task id: ");
            uartInt(current->task_id);
            uartPuts("  issue reschedule\n");

            for (int i = 0; i < 1000000; ++i)
            {
                asm volatile("nop");
            }

            current->re_schedule = false;
            schedule();
        }
    }
}