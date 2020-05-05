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

	if (current == 0)
	{
		current = next;
		switchToWOS(&next->kernel_context);
	}

	if (current->task_state == ready)
		pushQueue(current);

	struct task *prev = current;
	current = next;

	switchTo(&prev->kernel_context, &next->kernel_context);

	return;
}

void schedule()
{
	struct task *next_task = popQueue();

	if (next_task == -1)
		return;

	// uartPuts("Reschedule from ");
	// uartInt(current->task_id);
	// uartPuts(" to ");
	// uartInt(next_task->task_id);
	// uartPuts("\n");

	contextSwitch(next_task);

	return;
}

void checkRSFlag()
{
	if (current->re_schedule == true)
	{
		schedule();
	}

	return;
}