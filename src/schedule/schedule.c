#include "type.h"
#include "device/uart.h"
#include "schedule/task.h"
#include "schedule/switch.h"

const static unsigned int MAX_TASK_NUM = 64;
struct task task_pool[64];
char kstack_pool[64][4096];
char ustack_pool[64][4096];
int task_num = 0;
struct task* current;

void do_exec(void(*func)())
{
	current->user_context.pc = (unsigned long)func;
	current->user_context.sp = (unsigned long)&ustack_pool[task_num];

	switchToEL0(&current->user_context);
}

int createPrivilegeTask(void(*func)(), int priority)
{
	task_pool[task_num].kernel_context.pc = (unsigned long)func;
	task_pool[task_num].kernel_context.sp = (unsigned long)&kstack_pool[task_num];
	task_pool[task_num].task_id = task_num;
	task_pool[task_num].priority = priority;
	task_pool[task_num].task_state = ready;
	task_pool[task_num].re_schedule = false;
	task_num++;

	return task_num-1;
}

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