#include "type.h"
#include "device/uart.h"
#include "task/taskStruct.h"
#include "task/switch.h"

const static unsigned int MAX_TASK_NUM = 64;
struct task task_pool[64];
char kstack_pool[64][4096];
char ustack_pool[64][4096];
int task_num = 0;
struct task* current;

void _copyContext(struct task* destination, struct task* source)
{
	destination->kernel_context = source->kernel_context;
	destination->user_context = source->user_context;

	return;
}

void _sysFork()
{
    _copyContext(&task_pool[task_num], current);
	task_pool[task_num].kernel_context.sp = (unsigned long)&kstack_pool[task_num];
	task_pool[task_num].task_id = task_num;
	task_pool[task_num].priority = current->priority;
	task_pool[task_num].task_state = ready;
	task_pool[task_num].re_schedule = false;
	task_num++;

	*(unsigned int *)(current->kernel_context.sp-32*8) = 0;
	*(unsigned int *)task_pool[task_num].kernel_context.sp = task_num-1;

	return;
}

void doExec(void(*func)())
{
	current->user_context.pc = (unsigned long)func;
	current->user_context.sp = (unsigned long)&ustack_pool[task_num];

	switchToEL0(&current->kernel_context, &current->user_context);

	return;
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