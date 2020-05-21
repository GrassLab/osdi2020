#include "../include/mm.h"
#include "arm/mmu.h"
#include "sched.h"
#include "fork.h"
#include "utils.h"
#include "entry.h"

int copy_process(unsigned long clone_flags, unsigned long fn, unsigned long arg)
{
	preempt_disable();
	struct task_struct *p;

	unsigned long page = allocate_kernel_page(); // this function will return the free virtual page address
	p = (struct task_struct *) page;
	struct pt_regs *childregs = task_pt_regs(p);

	if (!p)
		return -1;

	if (clone_flags & PF_KTHREAD) {
		p->cpu_context.x19 = fn;
		p->cpu_context.x20 = arg;
	} else {
		struct pt_regs * cur_regs = task_pt_regs(current);
		*cur_regs = *childregs;
		childregs->regs[0] = 0;
		copy_virt_memory(p); // this function will copy pages, including the tables... 
	}
	p->flags = clone_flags;
	p->priority = current->priority;
	p->state = TASK_RUNNING;
	p->counter = p->priority;
	p->preempt_count = 1; //disable preemtion until schedule_tail

	p->cpu_context.pc = (unsigned long)ret_from_fork;
	p->cpu_context.sp = (unsigned long)childregs;
	
	int pid = nr_tasks++;
	p->task_id = pid;
	task[pid] = p;	

	preempt_enable();
	return pid;
}

int privilege_task_create(unsigned long start, unsigned long size, unsigned long pc)
{
	struct pt_regs *regs = task_pt_regs(current);
	regs->pstate = PSR_MODE_EL0t;
	regs->pc = pc + USER_TEXT_OFFSET; // for return to user mode virtual address
	regs->sp = 3 * PAGE_SIZE + USER_TEXT_OFFSET;  // first page is for code , second page is for stack
	unsigned long code_page = allocate_user_page(current, USER_TEXT_OFFSET, MMU_PTE_FLAGS); // allocate a user page and update tables
	unsigned long code_page2 = allocate_user_page(current, USER_TEXT_OFFSET + PAGE_SIZE, MMU_PTE_FLAGS); // second free page for code
	if (code_page == 0)	{
		return -1;
	}
	memcpy(start, code_page, PAGE_SIZE); // load the image code to allocated page
	memcpy(start + PAGE_SIZE, code_page2, size - PAGE_SIZE); // load the image code to allocated page
	set_pgd(current->mm.pgd); //change to user page but still can access kernel pages in kernel mode
	return 0;
}

struct pt_regs * task_pt_regs(struct task_struct *tsk)
{
	unsigned long p = (unsigned long)tsk + THREAD_SIZE - sizeof(struct pt_regs);
	return (struct pt_regs *)p;
}
