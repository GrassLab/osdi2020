#include "type.h"
#include "schedule/task.h"
#include "schedule/switch.h"

const static unsigned int MAX_TASK_NUM = 64;
struct task task_pool[64];
char kstack_pool[64][4096];
int task_num = 0;
struct task* current;

int createPrivilegeTask(void(*func)(), int priority)
{
	task_pool[task_num].cpu_context.pc = (unsigned long)func;
	task_pool[task_num].cpu_context.sp = (unsigned long)&kstack_pool[task_num];
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
    switchTo(prev, next);
}

void schedule()
{
	int next_task_id;
	int highest_priority = 0;
	for (int i = 0; i < task_num; ++i)
	{
		if (task_pool[i].task_state == ready && task_pool[i].priority >= highest_priority) 
		{
			next_task_id = i;
		}
	}

	contextSwitch(&task_pool[next_task_id]);

	return;
}