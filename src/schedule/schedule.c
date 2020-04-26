#include "type.h"
#include "device/uart.h"
#include "schedule/task.h"
#include "schedule/switch.h"

const static unsigned int MAX_ktask_num = 64;
struct task ktask_pool[64];
char kstack_pool[64][4096];
int ktask_num = 0;
struct task utask_pool[64];
char ustack_pool[64][4096];
int utask_num = 0;
struct task* current;

void do_exec(void(*func)(), int priority)
{
	utask_pool[utask_num].cpu_context.pc = (unsigned long)func;
	utask_pool[utask_num].cpu_context.sp = (unsigned long)&ustack_pool[utask_num];
	utask_pool[utask_num].task_id = utask_num;
	utask_pool[utask_num].priority = priority;
	utask_pool[utask_num].task_state = running;
	utask_pool[utask_num].re_schedule = false;
	utask_num++;

	current->task_state = ready;
	struct task* prev = current;
	current = &ustack_pool[utask_num-1];

	switchToEL0(prev, current);
}

int createPrivilegeTask(void(*func)(), int priority)
{
	ktask_pool[ktask_num].cpu_context.pc = (unsigned long)func;
	ktask_pool[ktask_num].cpu_context.sp = (unsigned long)&kstack_pool[ktask_num];
	ktask_pool[ktask_num].task_id = ktask_num;
	ktask_pool[ktask_num].priority = priority;
	ktask_pool[ktask_num].task_state = ready;
	ktask_pool[ktask_num].re_schedule = false;
	ktask_num++;

	return ktask_num-1;
}

void contextSwitch(struct task* next, bool returnEL0)
{
    if (current == next) 
		return;

	current->task_state = ready;
	next->task_state = running;

	struct task* prev = current;
	current = next;

	if (returnEL0)
		switchToEL0(prev, next);
	else
    	switchTo(prev, next);
}

int _getHighestKProc()
{
	int next_task_id;
	int highest_priority = 0;
	for (int i = 0; i < ktask_num; ++i)
	{
		if (ktask_pool[i].task_state == ready && ktask_pool[i].priority >= highest_priority) 
		{
			next_task_id = i;
		}
	}

	return next_task_id;
}

int _getHighestUProc()
{
	int next_task_id;
	int highest_priority = 0;
	for (int i = 0; i < utask_num; ++i)
	{
		if (utask_pool[i].task_state == ready && utask_pool[i].priority >= highest_priority) 
		{
			next_task_id = i;
		}
	}

	return next_task_id;
}

void schedule()
{
	int next_task_id = _getHighestKProc();

	contextSwitch(&ktask_pool[next_task_id], false);

	return;
}

void userScheduel()
{
	int next_task_id = _getHighestUProc();

	contextSwitch(&utask_pool[next_task_id], true);

	return;
}

void checkRSFlag()
{
	if (current->re_schedule == true)
	{
		uartPuts("reschedule\n");
		userScheduel();
	}

	return;
}