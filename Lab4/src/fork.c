#include "include/mm.h"
#include "include/entry.h"
#include "include/scheduler.h"

int privilege_task_create(void(* fn),unsigned long arg){
	
	struct task_struct *p;

	p = (struct task_struct *) get_free_page();
	if (!p)
		return 1;

	p->state = TASK_RUNNING;
	p->counter = 1;

	p->cpu_context.x19 = (unsigned long)fn;
	p->cpu_context.x20 = arg;
	p->cpu_context.pc = (unsigned long)ret_from_fork;
	p->cpu_context.sp = (unsigned long)p + THREAD_SIZE; //4096=4K for every thread
	int pid = nr_tasks++;
	task[pid] = p;	

	return 0;
}
