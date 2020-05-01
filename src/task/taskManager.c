#include "type.h"
#include "device/uart.h"
#include "task/taskStruct.h"
#include "task/switch.h"
#include "task/schedule.h"
#include "task/taskQueue.h"

const static uint32_t MAX_TASK_NUMBER = 64;
struct task task_pool[64];
char kstack_pool[64][4096];
char ustack_pool[64][4096];
uint32_t task_count = 0;
bool pool_occupied[64] = {false};
struct task *current;

int32_t _get_free_pool_num()
{
	if (task_count >= MAX_TASK_NUMBER)
		return -1;
	else
	{
		for (int32_t i = 0; i < MAX_TASK_NUMBER; ++i)
		{
			if (pool_occupied[i] == false)
			{
				task_count++;
				pool_occupied[i] = true;
				return i;
			}
		}
	}
}

void _sysFork()
{
	int32_t free_pool_num = _get_free_pool_num();

	if (free_pool_num == -1)
		return;

	struct task *new_task = &task_pool[free_pool_num];
	unsigned long new_kstask = (unsigned long)&kstack_pool[free_pool_num];

	uint32_t sp_begin;
	uint32_t sp_end;
	asm volatile("mov %0, x9"
				 : "=r"(sp_begin));
	asm volatile("mov %0, sp"
				 : "=r"(sp_end));
	uint32_t stack_size = sp_begin - sp_end;

	// Set parent trapframe
	*(uint32_t *)(sp_begin - 32 * 8) = free_pool_num;

	new_task->kernel_context.sp = new_kstask;
	// Copy the kernel stack
	copyStack(new_task->kernel_context.sp, sp_begin, stack_size);

	// Set child trapframe
	*(uint32_t *)(new_task->kernel_context.sp - 32 * 8) = 0;
	// Because the stack is copied, the stack pointer should be moved down
	new_task->kernel_context.sp -= stack_size;

	new_task->task_id = free_pool_num;
	new_task->priority = current->priority;
	new_task->task_state = ready;
	new_task->re_schedule = false;

	pushQueue(new_task);

	// Copy the context at last because I don't want to let child do the above works
	copyContext(&new_task->kernel_context);

	return;
}

void doExec(void (*func)())
{
	current->kernel_context.elr_el1 = (unsigned long)func;
	// Allocate new user stack
	current->kernel_context.sp_el0 = (unsigned long)&ustack_pool[current->task_id];

	switchToEL0(&current->kernel_context);

	return;
}

void _sysexec()
{
	uint32_t sp_begin;
	asm volatile("mov %0, x9"
				 : "=r"(sp_begin));
	uint32_t func = *(uint32_t *)(sp_begin - 32 * 8);

	doExec(func);
}

void _sysexit()
{
	int32_t sp_begin;
	asm volatile("mov %0, x9"
				 : "=r"(sp_begin));
	int32_t exit_code = *(int32_t *)(sp_begin - 32 * 8);

	uartPuts("Exit with code ");
	uartInt(exit_code);
	uartPuts("\n");

	current->task_state = zombie;
	task_count--;
}

void zombieReaper()
{
	while (1)
	{
		for (uint32_t i = 0; i < MAX_TASK_NUMBER; ++i)
		{
			if (task_pool[i].task_state == zombie)
				pool_occupied[i] = false;
		}

		schedule();
	}
}

uint32_t createPrivilegeTask(void (*func)(), uint32_t priority)
{
	int32_t free_pool_num = _get_free_pool_num();

	if (free_pool_num == -1)
		return;

	struct task *new_task = &task_pool[free_pool_num];
	unsigned long new_kstask = (unsigned long)&kstack_pool[free_pool_num];

	new_task->kernel_context.pc = (unsigned long)func;
	new_task->kernel_context.sp = new_kstask;
	new_task->task_id = free_pool_num;
	new_task->priority = priority;
	new_task->task_state = ready;
	new_task->re_schedule = false;

	pushQueue(new_task);

	return free_pool_num;
}