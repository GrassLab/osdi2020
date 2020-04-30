#include "type.h"
#include "device/uart.h"
#include "task/taskStruct.h"
#include "task/switch.h"

const static unsigned int MAX_TASK_NUM = 64;
struct task task_pool[64];
char kstack_pool[64][4096];
char ustack_pool[64][4096];
int task_num = 0;
struct task *current;

void _sysFork()
{
	if (task_num >= (MAX_TASK_NUM - 1)) // Task number > max task number
		return;

	unsigned int sp_begin;
	unsigned int sp_end;
	asm volatile("mov %0, x9"
				 : "=r"(sp_begin));
	sp_begin += 32 * 8;
	asm volatile("mov %0, sp"
				 : "=r"(sp_end));
	unsigned int stack_size = sp_begin - sp_end;

	*(unsigned int *)(sp_begin - 32 * 8) = task_num; // Set trapframe

	task_pool[task_num].kernel_context.sp = (unsigned long)&kstack_pool[task_num];
	copyStack(task_pool[task_num].kernel_context.sp, sp_begin, stack_size);
	*(unsigned int *)(task_pool[task_num].kernel_context.sp - 32 * 8) = 0; // Set trapframe
	task_pool[task_num].kernel_context.sp -= stack_size;

	task_pool[task_num].task_id = task_num;
	task_pool[task_num].priority = current->priority;
	task_pool[task_num].task_state = ready;
	task_pool[task_num].re_schedule = false;
	task_num++;

	copyContext(&task_pool[task_num - 1].kernel_context);

	return;
}

void _sysexec()
{
	unsigned int sp_begin;
	asm volatile("mov %0, x9"
				 : "=r"(sp_begin));
	sp_begin += 32 * 8;
	unsigned int func = *(unsigned int *)(sp_begin - 32 * 8);

	doExec(func);
}

void doExec(void (*func)())
{
	current->kernel_context.elr_el1 = (unsigned long)func;
	current->kernel_context.sp_el0 = (unsigned long)&ustack_pool[current->task_id];

	switchToEL0(&current->kernel_context);

	return;
}

int createPrivilegeTask(void (*func)(), int priority)
{
	task_pool[task_num].kernel_context.pc = (unsigned long)func;
	task_pool[task_num].kernel_context.sp = (unsigned long)&kstack_pool[task_num];
	task_pool[task_num].task_id = task_num;
	task_pool[task_num].priority = priority;
	task_pool[task_num].task_state = ready;
	task_pool[task_num].re_schedule = false;
	task_num++;

	return task_num - 1;
}