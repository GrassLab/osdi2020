#include "type.h"
#include "device/uart.h"
#include "task/taskStruct.h"
#include "task/switch.h"
#include "task/taskQueue.h"

const static uint32_t MAX_TASK_NUMBER = 64;
struct task task_pool[64];
char kstack_pool[64][4096];
char ustack_pool[64][4096];
uint32_t task_count = 0;
struct task *current;

void _sysFork()
{
	if (task_count >= MAX_TASK_NUMBER) // Task number > max task number
		return;

	struct task *new_task = &task_pool[task_count];
	unsigned long new_kstask = (unsigned long)&kstack_pool[task_count];

	uint32_t sp_begin;
	uint32_t sp_end;
	asm volatile("mov %0, x9"
				 : "=r"(sp_begin));
	asm volatile("mov %0, sp"
				 : "=r"(sp_end));
	uint32_t stack_size = sp_begin - sp_end;

	// Set parent trapframe
	*(uint32_t *)(sp_begin - 32 * 8) = task_count;

	new_task->kernel_context.sp = new_kstask;
	// Copy the kernel stack
	copyStack(new_task->kernel_context.sp, sp_begin, stack_size);

	// Set child trapframe
	*(uint32_t *)(new_task->kernel_context.sp - 32 * 8) = 0;
	// Because the stack is copied, the stack pointer should be moved down
	new_task->kernel_context.sp -= stack_size;

	new_task->task_id = task_count++;
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

uint32_t createPrivilegeTask(void (*func)(), uint32_t priority)
{
	if (task_count >= MAX_TASK_NUMBER) // Task number > max task number
		return -1;

	struct task *new_task = &task_pool[task_count];
	unsigned long new_kstask = (unsigned long)&kstack_pool[task_count];

	new_task->kernel_context.pc = (unsigned long)func;
	new_task->kernel_context.sp = new_kstask;
	new_task->task_id = task_count++;
	new_task->priority = priority;
	new_task->task_state = ready;
	new_task->re_schedule = false;

	pushQueue(new_task);

	return task_count - 1;
}