#include "mm.h"
#include "../include/sched.h"
#include "entry.h"
#include "printf.h"
#include "../include/fork.h"

int privilege_task_create(unsigned long clone_flags, unsigned long fn, unsigned long arg, unsigned long stack)
{
	preempt_disable();
	struct task_struct *p;
	unsigned long pid = get_kernel_id_page() + 1; //consider init kernel process
	
	p = (struct task_struct *) (LOW_MEMORY + (pid - 1) * PAGE_SIZE);
	
	if (!p)
		return -1;

	struct pt_regs *childregs = task_pt_regs(p);
	memzero((unsigned long) childregs, sizeof(struct pt_regs));
	memzero((unsigned long) &p->cpu_context, sizeof(struct cpu_context));

	if (clone_flags & PF_KTHREAD) {
		p->cpu_context.x19 = fn;
		p->cpu_context.x20 = arg;
	}
	else {
		struct pt_regs *cur_regs   = task_pt_regs(current);
		*childregs = *cur_regs;	// copy the parent kernel task
		if (current->task_id > pid) {
			childregs->regs[29] = cur_regs->regs[29] + get_user_page(current->task_id) - get_user_page(pid);
			childregs->sp       = cur_regs->sp + get_user_page(pid) - get_user_page(current->task_id);
		}
		else {
			childregs->regs[29] = cur_regs->regs[29] + get_user_page(pid) - get_user_page(current->task_id);
			childregs->sp       = cur_regs->sp + get_user_page(pid) - get_user_page(current->task_id);
		}
		childregs->regs[0] 	= 0; // fork return with 0
		memcpy((unsigned long)childregs->sp, (unsigned long)cur_regs->sp, ((LOW_USER_STACK + (pid - 1) * PAGE_SIZE) - (unsigned long)cur_regs->sp));
		p->stack = stack; // ?????
	}

	p->flags    = 		clone_flags;
	p->task_id  = 		pid;
	p->priority = 		current->priority;
	p->state 	= 		TASK_RUNNING;
	p->counter 	= 		p->priority;
	p->preempt_count = 	1; //disable preemtion until schedule_tail
	p->kill_flag = 		0;
	p->cpu_context.pc = (unsigned long)ret_from_fork;
	p->cpu_context.sp = (unsigned long)childregs;

	task[pid] = p;	//put process stack to run queue
	nr_tasks++;
	
	return pid;
}

int do_exec(unsigned long pc) 
{
	struct pt_regs *new_regs = task_pt_regs(current);
	memzero((unsigned long)new_regs, sizeof(*new_regs));
	new_regs->pc = pc;
	new_regs->pstate = PSR_MODE_EL0t;
	unsigned long stack = get_user_page(current->task_id);
	if (!stack) {
		return -1;
	}
	new_regs->sp = stack + PAGE_SIZE;
	current->stack = stack; ///??????????????????
	return 0;
}

struct pt_regs* task_pt_regs(struct task_struct *tsk) 
{
	unsigned long addr = (unsigned long)(tsk) + THREAD_SIZE - sizeof(struct pt_regs);
	return (struct pt_regs *)addr;
}