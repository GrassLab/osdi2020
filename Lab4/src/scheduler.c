#include "include/scheduler.h"
#include "include/uart.h"

static struct task_struct init_task = INIT_TASK;
struct task_struct *current = &(init_task);
struct task_struct * task[NR_TASKS] = {&(init_task), };
int nr_tasks = 1;

void _schedule(void)
{
	int next;
	int flag;

	struct task_struct * p;
	while (1) {
		next = 0;
		flag = 0;
		for (int i = 0; i < NR_TASKS; i++){
			p = task[i];
			if (p && p->state == TASK_RUNNING && p->counter != 0) {
				flag = 1;
				next = i;
			}
		}
		if(flag==1)
			break;
		// If no one has time slice to use, then reset their timeslice! 
		for (int i = 0; i < NR_TASKS; i++) {
			p = task[i];
			if (p) {
				p->counter = 1;
			}
		}
	}
	context_switch(task[next]);
}

void schedule(void)
{
	current->counter = 0;
	_schedule();
}

void context_switch(struct task_struct * next)
{
	if (current == next)
		return;
	struct task_struct * prev = current;
	current = next;
	switch_to(prev, next);
}



