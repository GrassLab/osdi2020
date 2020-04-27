#include "mm.h"
#include "../include/sched.h"
#include "entry.h"

int copy_process(unsigned long fn, unsigned long arg)
{
	preempt_disable();
	struct task_struct *p;
	unsigned long id = get_kernel_id_page();

	p = (struct task_struct *) (LOW_MEMORY + id * PAGE_SIZE);

	if (!p)
		return 1;

	p->task_id  = id + 1;
	p->priority = current->priority;
	p->state 	= TASK_RUNNING;
	p->counter 	= p->priority;
	p->preempt_count = 1; //disable preemtion until schedule_tail

	p->cpu_context.x19 = fn;
	p->cpu_context.x20 = arg;
	p->cpu_context.pc = (unsigned long)ret_from_fork;
	p->cpu_context.sp = (unsigned long)p + THREAD_SIZE;

	int pid = nr_tasks++;
	task[pid] = p;	//put process stack to run queue
	preempt_enable();
	return 0;
}