#include "type.h"
#include "device/uart.h"
#include "task/taskStruct.h"
#include "task/taskManager.h"
#include "task/switch.h"
#include "task/taskQueue.h"

void contextSwitch(struct task *next)
{
	if (current == next)
		return;

	pushQueue(current);

	struct task *prev = current;
	current = next;

	switchTo(&prev->kernel_context, &next->kernel_context);
}

void schedule()
{
	struct task *next_task = popQueue();

	if (next_task == -1)
		return;

	contextSwitch(next_task);

	return;
}

void checkRSFlag()
{
	if (current->re_schedule == true)
	{
		uartPuts("reschedule\n");
		schedule();
	}

	return;
}