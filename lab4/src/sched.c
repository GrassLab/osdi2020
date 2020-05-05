#include "irq.h"
#include "../include/sched.h"
#include "printf.h"

static struct task_struct init_task = INIT_TASK;
struct task_struct *current = &(init_task);
struct task_struct * task[NR_TASKS] = {&(init_task), };
int nr_tasks = 1;

void switch_to(struct task_struct * next) 
{
	if (current == next) 
		return;
	struct task_struct * prev = current;
	current = next;
	cpu_switch_to(prev, next);
}

void _schedule(void)
{
	int next,c;
	struct task_struct * p;
	while (1) {
		c = -1;
		next = 0;
		for (int i = 0; i < NR_TASKS; i++){
			p = task[i];
			if (p && p->state == TASK_RUNNING && p->counter > c) {
				c = p->counter;
				next = i;
			}
		}
		if (c) {
			break;
		}
		for (int i = 0; i < NR_TASKS; i++) {
			p = task[i];
			if (p) {
				p->counter = (p->counter >> 1) + p->priority;
			}
		}
	}
	if (task[next]->kill_flag == 1) {
		task[next]->state = TASK_ZOMBIE;
		nr_tasks -= 1;
		_schedule();
	}
	else {
		switch_to(task[next]);
	}
	return;
}

void schedule_uart(void)
{
	current->counter = 0;
	preempt_disable();
	int next,c;
	struct task_struct * p;
	while(1) {
		c = -1;
		next = 0;
		for (int i = 0; i < NR_TASKS; i++) {
			p = task[i];
			if (p && p->state == TASK_WAITING && p->counter > c) {
				c = p->counter;
				next = i;
			}
		}
		if (c == -1) {
			return; //not waiting task
		}
		if (c) {
			break;
		}
		for (int i = 0; i < NR_TASKS; i++) {
			p = task[i];
			if (p && p->state == TASK_WAITING) {
				p->counter = (p->counter >> 1) + p->priority;
			}
		}
	}
	task[next]->state = TASK_RUNNING;
	switch_to(task[next]);
	return;
}

void schedule(void)
{
	current->counter = 0;
	_schedule();
}

void schedule_tail(void) 
{
	//enable
	preempt_enable();
}

void preempt_disable(void)
{
	current->preempt_count++;
}

void preempt_enable(void)
{
	current->preempt_count--;
}
