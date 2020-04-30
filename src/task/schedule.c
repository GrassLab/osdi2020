#include "type.h"
#include "device/uart.h"
#include "task/taskStruct.h"
#include "task/taskManager.h"
#include "task/switch.h"

void contextSwitch(struct task *next)
{
	if (current == next)
		return;

	current->task_state = ready;
	next->task_state = running;

	struct task *prev = current;
	current = next;

	switchTo(&prev->kernel_context, &next->kernel_context);
}

int _getNextProc()
{
	for (int i = 0; i < task_num; ++i)
	{
		if (task_pool[i].task_state == ready)
		{
			return i;
		}
	}

	return -1;
}

void schedule()
{
	int next_task_id = _getNextProc();

	if (next_task_id == -1)
		return;

	contextSwitch(&task_pool[next_task_id]);

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