#include "type.h"
#include "device/uart.h"
#include "task/taskStruct.h"
#include "task/taskManager.h"
#include "task/switch.h"

void contextSwitch(struct task* next)
{
    if (current == next) 
		return;

	current->task_state = ready;
	next->task_state = running;

	struct task* prev = current;
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
		asm volatile("mrs x19, sp_el0");
		asm volatile("mrs x20, elr_el1");
		asm volatile("mrs x21, spsr_el1");
		
		uartPuts("reschedule\n");
		schedule();

		asm volatile("msr sp_el0, x19");
		asm volatile("msr elr_el1, x20");
		asm volatile("msr spsr_el1, x21");
	}

	return;
}