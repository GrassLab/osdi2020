#include "kernel/sched.h"
#include "kernel/task.h"
#include "kernel.h"
#include "mm.h"

#define POOL_SIZE 64
/* TASK STATE */
#define TASK_STATE_NEW 0
#define TASK_STATE_READY 1
#define TASK_STATE_RUNNING 2
#define TASK_STATE_WAIT 3
#define TASK_STATE_EXIT 4

#define PRIVILEDGE 1
#define NORMAL 0

uint64_t kstack_pool[POOL_SIZE][4096];
uint64_t stack_pool[POOL_SIZE][4096];

#define POOL_FULL(bitmap) (bitmap == 0xFFFFFFFFFFFFFFFF)
#define POOL_EMPTY(bitmap) (bitmap == 0x0)
#define TASK_OCCUPY(id) (bitmap & 1 << id)
#define TASK_ADD(id) (bitmap |= 1 << id)
#define TASK_INIT(id, sec, func)                                               \
	{                                                                      \
		task_pool[id].task_id = id;                                    \
		task_pool[id].ksp = (uint64_t)kstack_pool[id];                 \
		task_pool[id].kbase = (uint64_t)kstack_pool[id];               \
		task_pool[id].sp_el0 = (uint64_t)stack_pool[id];               \
		task_pool[id].spsr_el1 = 0;                                    \
		task_pool[id].elr_el1 = (uint64_t)func;                        \
		task_pool[id].ubase = (uint64_t)stack_pool[id];                \
		task_pool[id].privilege = sec;                                 \
		task_pool[id].state = TASK_STATE_NEW;                          \
		task_pool[id].fp_lr[1] = (uint64_t)func;                       \
	}

int32_t privilege_task_create(void (*func)())
{
	/* task pool full */
	if (POOL_FULL(bitmap))
		return -1;
	for (int32_t i = 0; i < POOL_SIZE; i++) {
		if (!TASK_OCCUPY(i)) {
			// struct mm_struct m = { .};
			TASK_INIT(i, PRIVILEDGE, func);
			// print("Task INIT\n");
			// print_task(&task_pool[i]);
			// print("\n");
			TASK_ADD(i);
			return i;
		}
	}
	return -1; //	means task pool full shouldn't come to this section if it's single core
}

void set_trap_ret(uint64_t retval, uint32_t id)
{
	task_pool[id].trapframe = retval;
}

void exec(void (*func)())
{
	asm volatile("mov x1, %0\n\t"
		     "mov x0, #13\n\t"
		     "svc #0" ::"r"(func)
		     :);
}
void sys_exec(int num, void (*func)())
{
	/* do non privilege */
	struct task_t *current = &task_pool[current_task];
	asm volatile("msr sp_el0, %0" ::"r"(current->sp_el0) :); // TODO:  ubase
	asm volatile("msr spsr_el1, %0" ::"r"(0) :);
	asm volatile("msr elr_el1, %0" ::"r"(func) :);
	asm volatile("eret");
}

void *memcpy(void *dest, const void *src, int32_t len)
{
	char *d = dest;
	const char *s = src;
	while (len--)
		*d++ = *s++;
	return dest;
}

int32_t do_fork()
{
	struct task_t *current = &task_pool[current_task];
	// int32_t child_pid = privilege_task_create(current->fp_lr[1]);
	// struct task_t *child = &task_pool[child_pid];
	// child->privilege = current->privilege;
	// child->elr_el1 = current->elr_el1;
	// child->spsr_el1 = current->spsr_el1;
	// child->sp_el0 -= (current->ubase - current->sp_el0);
	// memcpy((void *)child->sp_el0, (void *)current->sp_el0,
	//        current->ubase - current->sp_el0);
	// // fork_ret(&task_pool[child_pid], current->kbase, child->kbase);
	// // uint64_t sp;
	// // asm volatile("mov %0, sp" : "=r"(sp)::);
	// // memcpy((void *)child->kbase, (void *)current->kbase,
	// //        current->kbase - sp);
	// current->trapframe = child_pid;
	// child->trapframe = 0;
	uint32_t child_pid = -1;
	uint64_t pc;
	asm volatile("mrs %0, elr_el1" : "=r"(pc)::);
	if (POOL_FULL(bitmap))
		return -1;
	for (int32_t i = 0; i < POOL_SIZE; i++) {
		if (!TASK_OCCUPY(i)) {
			TASK_ADD(i);
			child_pid = i;
			break;
		}
	}
	if (child_pid == -1) {
		return -1;
	} else {
		struct task_t *child = &task_pool[child_pid];
		uint64_t frame_ptr;
		asm volatile("mov %0, x29" : "=r"(frame_ptr)::);
		*child = *current;
		child->task_id = child_pid;
		child->ksp = (uint64_t)kstack_pool[child_pid]; // have issue
		child->kbase = (uint64_t)kstack_pool[child_pid];
		child->ksp -= (current->kbase - frame_ptr);
		memcpy((void *)child->kbase, (void *)current->kbase,
		       current->kbase - frame_ptr);
		child->sp_el0 = (uint64_t)stack_pool[child_pid];
		child->fp_lr[1] = (uint64_t)pc;
		child->fp_lr[0] =
			current->fp_lr[0] + (child->sp_el0 - current->sp_el0);
		// child->fp_lr[1] = current->fp_lr[1];
		child->ubase = (uint64_t)stack_pool[child_pid];
		// asm volatile("mrs %0, spsr_el1" ::"r"(child->spsr_el1) :);
		child->spsr_el1 = current->spsr_el1;
		child->elr_el1 = current->elr_el1;
		child->sp_el0 -=
			current->ubase - current->sp_el0; //TODO: GDB check
		memcpy((void *)child->sp_el0, (void *)current->sp_el0,
		       current->ubase - current->sp_el0);
		/* setup trapframe */
		current->trapframe = child_pid;
		child->trapframe = 0;
		return child_pid;
	}
}

struct task_t *get_running_task()
{
	struct task_t *current;
	asm volatile("mrs %0, tpidr_el1" : "=r"(current)::);
	return current;
}

void print_task(struct task_t *task)
{
	print("PID => %d\n", task->task_id);
	print("PRIVILEGE => %s\n", task->privilege ? "KERNEL" : "USER");
	print("SP_EL0 => %x\n", task->sp_el0);
	print("ELR_EL1 => %x\n", task->elr_el1);
	print("FP_LR => %x && %x\n", task->fp_lr[0], task->fp_lr[1]);
	for (int i = 0; i < 10; i++) {
		print("x%d => %x\n", i + 19, task->saved_reg[i]);
	}
}