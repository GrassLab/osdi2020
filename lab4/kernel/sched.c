#include "kernel/task.h"
#include "kernel.h"
#include "uart.h"

#define POOL_SIZE 64
#define TASK_STATE_NEW 0
#define TASK_STATE_READY 1
#define TASK_STATE_RUNNING 2
#define TASK_STATE_WAIT 3
#define TASK_STATE_EXIT 4
#define POOL_FULL(bitmap) (bitmap == 0xFFFFFFFFFFFFFFFF)
#define POOL_EMPTY(bitmap) (bitmap == 0x0)

void context_switch(struct task_t *prev, struct task_t *next);

volatile uint64_t bitmap;
struct task_t task_pool[POOL_SIZE];
struct task_t fake_task = { .task_id = -1 };
volatile uint32_t current_task;
int8_t resched;

void sched_init()
{
	resched = 1;
	current_task = -1;
	// asm volatile("msr tpidr_el1, %0" ::"r"(0) :);
	// asm volatile("mov sp, %0" ::"r"(task_pool[0].ksp) :);
}

uint32_t get_el()
{
	uint32_t el;
	asm volatile("mrs %0, currentEL" : "=r"(el)::);
	return el >> 2;
}
void enable_irq();
void disable_irq();

void sched_next()
{
	uint32_t next_task;
	if (!resched) {
		return;
	}
	// resched = 0;
	if (POOL_EMPTY(bitmap))
		return; // TODO: put a wfe task?
	for (next_task = (current_task + 1) % POOL_SIZE;
	     next_task != current_task;
	     next_task = (next_task + 1) % POOL_SIZE) {
		if ((bitmap >> next_task) & 0x1) {
			break;
		}
	}
	if (current_task == next_task) {
		return;
	} else {
		// switch to next_task
		struct task_t *prev = current_task == -1 ?
					      &fake_task :
					      &task_pool[current_task];
		struct task_t *next = &task_pool[next_task];
		current_task = next_task;
		asm volatile("mrs %0, spsr_el1" : "=r"(prev->spsr_el1)::);
		asm volatile("mrs %0, elr_el1" : "=r"(prev->elr_el1)::);
		asm volatile("mrs %0, sp_el0" : "=r"(prev->sp_el0)::);
		// print("BEFORE CTX\n");
		// print_task(prev);
		// print_task(next);
		print("CTX from %d to %d.\n", prev->task_id, next->task_id);
		context_switch(prev, next);
		// print("AFTER CTX\n");
		// print_task(prev);
		// print_task(next);
		next = &task_pool[current_task];
		asm volatile("msr spsr_el1, %0" ::"r"(next->spsr_el1) :);
		asm volatile("msr elr_el1, %0" ::"r"(next->elr_el1) :);
		asm volatile("msr sp_el0, %0" ::"r"(next->sp_el0) :);
	}
}

void sys_exit()
{
	bitmap &= ~(1 << current_task);
	uart_hex(bitmap);
	sched_next();
}