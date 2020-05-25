#include "type.h"
#include "device/uart.h"
#include "task/taskStruct.h"
#include "task/switch.h"
#include "task/schedule.h"
#include "task/taskQueue.h"
#include "memory/memManager.h"

const static uint32_t MAX_TASK_NUMBER = 64;
struct task* task_pool[64];
uint32_t task_count = 0;
uint64_t pool_occupied = 0;
struct task *current = 0;

int32_t _getFreePoolNum()
{
	if (task_count < MAX_TASK_NUMBER)
	{
		for (int32_t i = 0; i < MAX_TASK_NUMBER; ++i)
		{
			if (!((pool_occupied >> i) & 1))
			{
				task_count++;
				pool_occupied |= (1 << i);
				return i;
			}
		}
	}

	return -1;
}

void _sysGetTaskId()
{
	uint64_t sp_begin = current->kernel_context.ksp_begin;
	*(uint64_t *)(sp_begin - 32 * 8) = current->task_id;

	return;
}

void _sysFork()
{
	int32_t free_pool_num = _getFreePoolNum();

	if (free_pool_num == -1)
		return;

	struct task *new_task;
	uint64_t page = allocateKernelPage();	
	new_task = (struct task*) page;
	new_task->mm.kernel_pages[++new_task->mm.kernel_pages_count] = page;

	page = allocateKernelPage();	
	uint64_t new_kstack = page + 0x1000;
	new_task->kernel_context.ksp_begin = new_kstack;
	new_task->mm.kernel_pages[++new_task->mm.kernel_pages_count] = page;

	uint64_t new_ustack = 0xffffffffd000;
	new_task->kernel_context.usp_begin = 0xffffffffd000;

	uint64_t ksp_begin = current->kernel_context.ksp_begin;
	uint64_t ksp_end;
	asm volatile("mov %0, sp"
				 : "=r"(ksp_end));
	uint64_t kstack_used = ksp_begin - ksp_end;

	uint64_t usp_begin = current->kernel_context.usp_begin;
	uint64_t usp_end;
	asm volatile("mrs %0, sp_el0"
				 : "=r"(usp_end));
	uint64_t ustack_used = usp_begin - usp_end;
	uint64_t ufp = *(uint64_t *)(ksp_begin - 3 * 8);
	uint64_t ufp_offset = usp_end - ufp;

	// Set parent return value
	*(uint64_t *)(ksp_begin - 32 * 8) = free_pool_num;

	// Copy the kernel stack
	copyStack(new_kstack, ksp_begin, 4096);
	// copyStack(new_ustack, usp_begin, 4096);

	// Set child return value
	*(uint64_t *)(new_kstack - 32 * 8) = 0;
	// Set child fp
	*(uint64_t *)(new_kstack - 3 * 8) = new_ustack - ustack_used + ufp_offset;
	// Because the stack is copied, the stack pointer should be moved down
	new_task->kernel_context.sp = new_kstack - kstack_used;
	new_task->kernel_context.sp_el0 = new_ustack - ustack_used;

	new_task->task_id = free_pool_num;
	new_task->priority = current->priority;
	new_task->task_state = ready;
	new_task->re_schedule = false;
	new_task->mm.pgd = current->mm.pgd;

	pushQueue(new_task);

	// Copy the context at last because I don't want to let child do the above works
	copyContext(&new_task->kernel_context);

	return;
}

void doExec(uint64_t start, uint64_t size, uint64_t pc)
{
	current->kernel_context.elr_el1 = pc;
	// Allocate new user stack
	current->kernel_context.sp_el0 = 0xffffffffe000;
	current->kernel_context.usp_begin = 0xffffffffe000;
	allocateUserPage(current, 0xffffffffd000);
	allocateUserPage(current, 0xffffffffc000);

	uint64_t code_page = allocateUserPage(current, pc);
	memcpy(code_page, start, size);

	setPgd(current->mm.pgd);
	switchToEL0(&current->kernel_context);

	return;
}

void _sysexec()
{
	// uint64_t sp_begin;
    // asm volatile("mov %0, x7"
    //              : "=r"(sp_begin));
	// uint64_t func = *(uint64_t *)(sp_begin - 32 * 8);

	// doExec((void (*)(void))func);
}

void zombieReaper()
{
	while (1)
	{
		for (uint32_t i = 0; i < MAX_TASK_NUMBER; ++i)
		{
			if (task_pool[i]->task_state == zombie)
			{
				pool_occupied &= ~(1 << i);
			}
		}

		schedule();
	}
}

void _sysexit()
{
	uint64_t sp_begin = current->kernel_context.ksp_begin;
	int64_t exit_code = *(int64_t *)(sp_begin - 32 * 8);

	uartPuts("Exit with code ");
	uartInt(exit_code);
	uartPuts("\n");

	current->task_state = zombie;
	task_count--;

	// for (int i = 0, end = current->mm.kernel_pages_count; i < end; ++i)
	// 	freePage(current->mm.kernel_pages[i] - VA_START);
	// for (int i = 0, end = current->mm.user_pages_count; i < end; ++i)
	// 	freePage(current->mm.user_pages[i].phys_addr);

	schedule();
}

uint32_t createPrivilegeTask(void (*func)(), uint32_t priority)
{
	int32_t free_pool_num = _getFreePoolNum();

	if (free_pool_num == -1)
		return -1;

	struct task *new_task;
	uint64_t page = allocateKernelPage();	

	new_task = (struct task*) page;
	new_task->mm.kernel_pages[++new_task->mm.kernel_pages_count] = page;

	task_pool[free_pool_num] = new_task;

	page = allocateKernelPage();
	new_task->mm.kernel_pages[++new_task->mm.kernel_pages_count] = page;

	new_task->kernel_context.pc = (uint64_t)func;
	new_task->kernel_context.sp = page + 0x1000;
	new_task->kernel_context.ksp_begin = page + 0x1000;
	new_task->task_id = free_pool_num;
	new_task->priority = priority;
	new_task->task_state = ready;
	new_task->re_schedule = false;

	pushQueue(new_task);

	return free_pool_num;
}