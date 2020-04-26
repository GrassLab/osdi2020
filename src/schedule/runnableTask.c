#include "type.h"
#include "device/uart.h"
#include "schedule/schedule.h"

void idleTask()
{
	while (1)
	{
		schedule();
	}
}

void userTask()
{
    while (1)
	{
    }
}

void execTask()
{
    do_exec(&userTask, 1);
}

void kernelTask1()
{
    while (1)
    {
        if (current->re_schedule == true)
        {
            uartPuts("task1... issue reschedule\n");

            current->re_schedule = false;
            schedule();

            // for (int i = 0; i < 15000000; ++i)
            // {
            //     asm volatile("nop");
            // }
        }
    }
}

void kernelTask2()
{
    while (1)
    {
        if (current->re_schedule == true)
        {
            uartPuts("task2... issue reschedule\n");

            current->re_schedule = false;
            schedule();

            // for (int i = 0; i < 15000000; ++i)
            // {
            //     asm volatile("nop");
            // }
        }
    }
}