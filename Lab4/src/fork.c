#include "include/fork.h"
#include "include/mm.h"
#include "include/entry.h"
#include "include/scheduler.h"
#include "include/queue.h"

int privilege_task_create(void(* fn),unsigned long arg){
	preempt_disable();
	struct task_struct *p;

	p = (struct task_struct *) get_free_page();
	if (!p)
		return 1;

	struct pt_regs *childregs = task_pt_regs(p);
	memzero((unsigned long)childregs, sizeof(struct pt_regs));
	memzero((unsigned long)&p->cpu_context, sizeof(struct cpu_context));
	
	p->cpu_context.x19 = (unsigned long)fn;
	p->cpu_context.x20 = arg;
	
	p->state = TASK_RUNNING;
	p->priority = 1;
	p->counter = 1;
	p->preempt_lock = 1;

	p->cpu_context.pc = (unsigned long)ret_from_fork;
	p->cpu_context.sp = (unsigned long)childregs; 
	int pid = nr_tasks++;
	task[pid] = p;	
	p->pid = pid;

	runQ_push(runQ,&runQ_tail,pid);
	preempt_enable();
	return pid;
}

int fork(unsigned long stack)
{
	preempt_disable();
	struct task_struct *p;

	p = (struct task_struct *) get_free_page();
	if (!p) {
		return -1;
	}

	struct pt_regs *childregs = task_pt_regs(p);
	memzero((unsigned long)childregs, sizeof(struct pt_regs));
	memzero((unsigned long)&p->cpu_context, sizeof(struct cpu_context));
	
	
	struct pt_regs * cur_regs = task_pt_regs(current);
	*childregs = *cur_regs;
	childregs->regs[0] = 0; //x0 in the new state is set to 0, because x0 will be interpreted by the caller as a return value of the syscall.
	childregs->sp = stack + PAGE_SIZE;
	p->stack = stack;
	
	p->cpu_context.x19 = 0; // set this for return to user in entry.S
		
	p->priority = current->priority;
	p->state = TASK_RUNNING;
	p->counter = p->priority;
	p->preempt_lock = 1; //disable preemtion until schedule_tail

	p->cpu_context.pc = (unsigned long)ret_from_fork;
	p->cpu_context.sp = (unsigned long)childregs;
	int pid = nr_tasks++;
	task[pid] = p;
	p->pid = pid;

	runQ_push(runQ,&runQ_tail,pid);
	preempt_enable();
	return pid;
}


struct pt_regs * task_pt_regs(struct task_struct *tsk){
	 //a small area (pt_regs area) was reserved at the top of the stack of the newly created task.
	unsigned long p = (unsigned long)tsk + THREAD_SIZE - sizeof(struct pt_regs);
	return (struct pt_regs *)p;
}

int do_exec(unsigned long pc)
{
	struct pt_regs *regs = task_pt_regs(current);
	memzero((unsigned long)regs, sizeof(*regs));

	regs->elr_el1 = pc;             // copy to elr_el1 
	regs->spsr_el1 = 0x00000000; // copy to spsr_el1 for enter el0 
	
	unsigned long stack = get_free_page(); //allocate new user stack
	if (!stack) {
		return -1;
	}
	regs->sp = stack + PAGE_SIZE;

	current->stack = stack;
	return 0;
}
