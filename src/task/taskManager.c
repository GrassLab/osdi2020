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

int32_t _getFreePoolNum()
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

void _sysGetTaskId()
{
	uint32_t sp_begin = &kstack_pool[current->task_id + 1];
	*(uint32_t *)(sp_begin - 32 * 8) = current->task_id;

	return;
}

void _sysFork()
{
	int32_t free_pool_num = _getFreePoolNum();

	if (free_pool_num == -1)
		return;

	struct task *new_task = &task_pool[free_pool_num];
	uint64_t new_kstack = (uint64_t)&kstack_pool[free_pool_num + 1];
	uint64_t new_ustack = (uint64_t)&ustack_pool[free_pool_num + 1];

	uint32_t ksp_begin = &kstack_pool[current->task_id + 1];
	uint32_t ksp_end;
	asm volatile("mov %0, sp"
				 : "=r"(ksp_end));
	uint32_t kstack_used = ksp_begin - ksp_end;

	uint32_t usp_begin = &ustack_pool[current->task_id + 1];
	uint32_t usp_end;
	asm volatile("mrs %0, sp_el0"
				 : "=r"(usp_end));
	uint32_t ustack_used = usp_begin - usp_end;
	uint32_t ufp = *(uint32_t *)(ksp_begin - 3 * 8);
	uint32_t ufp_offset = usp_end - ufp;

	// Set parent return value
	*(uint32_t *)(ksp_begin - 32 * 8) = free_pool_num;

	// Copy the kernel stack
	copyStack(new_kstack, &kstack_pool[current->task_id + 1], kstack_used);
	copyStack(new_ustack, &ustack_pool[current->task_id + 1], 4096);

	// Set child return value
	*(uint32_t *)(new_kstack - 32 * 8) = 0;
	// Set child fp
	*(uint32_t *)(new_kstack - 3 * 8) = new_ustack - ustack_used + ufp_offset;
	// Because the stack is copied, the stack pointer should be moved down
	new_task->kernel_context.sp = new_kstack - kstack_used;
	new_task->kernel_context.sp_el0 = new_ustack - ustack_used;

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
	current->kernel_context.elr_el1 = (uint64_t)func;
	// Allocate new user stack
	current->kernel_context.sp_el0 = (uint64_t)&ustack_pool[current->task_id + 1];

	switchToEL0(&current->kernel_context);

	return;
}

void _sysexec()
{
	uint32_t sp_begin = &kstack_pool[current->task_id + 1];
	uint32_t func = *(uint32_t *)(sp_begin - 32 * 8);

	doExec(func);
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

void _sysexit()
{
	int32_t sp_begin = &kstack_pool[current->task_id + 1];
	int32_t exit_code = *(int32_t *)(sp_begin - 32 * 8);

	uartPuts("Exit with code ");
	uartInt(exit_code);
	uartPuts("\n");

	current->task_state = zombie;
	task_count--;

	schedule();
}

uint32_t createPrivilegeTask(void (*func)(), uint32_t priority)
{
	int32_t free_pool_num = _getFreePoolNum();

	if (free_pool_num == -1)
		return;

	struct task *new_task = &task_pool[free_pool_num];
	uint64_t new_kstask = (uint64_t)&kstack_pool[free_pool_num + 1];

	new_task->kernel_context.pc = (uint64_t)func;
	new_task->kernel_context.sp = new_kstask;
	new_task->task_id = free_pool_num;
	new_task->priority = priority;
	new_task->task_state = ready;
	new_task->re_schedule = false;

	pushQueue(new_task);

	return free_pool_num;
}