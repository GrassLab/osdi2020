#include "type.h"
#include "device/uart.h"
#include "schedule/schedule.h"

// User task

void userTask()
{
    while (1)
	{
    }
}

// Kernel task

void execTask()
{
    do_exec(&userTask, 1);
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