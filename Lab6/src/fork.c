#include "include/fork.h"
#include "include/mm.h"
#include "include/entry.h"
#include "include/scheduler.h"
#include "include/queue.h"
#include "include/mm.h"
#include "include/utils.h"
#include "include/kernel.h"
#include "include/elf.h"

static unsigned short pid_map[64] = {0,};

int get_availible_pid(){
	// pid 0 is preserve for IDLE_TASK
	for(int i=1;i<64;i++){
		if(pid_map[i]==0){
			pid_map[i] = 1;
			return i;
		}
	}
	return -1;
}

void free_pid(int i){
	pid_map[i]=0;
}

int privilege_task_create(void(* fn),int priority){
	preempt_disable();
	struct task_struct *p;

	p = (struct task_struct *) allocate_kernel_page(0);
	if (!p)
		return -1;
	
	struct trapframe *childregs = get_task_trapframe(p);
	
	memzero((unsigned long)childregs, sizeof(struct trapframe));
	memzero((unsigned long)&p->cpu_context, sizeof(struct cpu_context));
	memzero((unsigned long)&p->mm, sizeof(struct mm_struct));
	
	p->cpu_context.x19 = (unsigned long)fn;
	
	p->state = TASK_RUNNING;
	p->priority = priority;
	p->counter = p->priority;
	p->preempt_lock = 1;

	p->cpu_context.pc = (unsigned long)ret_from_fork;
	p->cpu_context.sp = (unsigned long)childregs; 

	int pid = get_availible_pid();
	task[pid] = p;	
	p->pid = pid;

	priorityQ_push(&runqueue,p->priority,p->pid);	
	preempt_enable();
	return pid;
}

int user_task_create()
{
	preempt_disable();
	struct task_struct *p;

	p = (struct task_struct *) allocate_kernel_page(0);
	if (!p) 
		return -1;
	
	struct trapframe *childregs = get_task_trapframe(p);
	memzero((unsigned long)childregs, sizeof(struct trapframe));
	memzero((unsigned long)&p->cpu_context, sizeof(struct cpu_context));
	memzero((unsigned long)&p->mm, sizeof(struct mm_struct));
	
	struct trapframe * cur_regs = get_task_trapframe(current);
	*childregs = *cur_regs; //copy content of parent register
	childregs->regs[0] = 0; //x0 in the new state is set to 0, because x0 will be interpreted by the caller as a return value of the syscall.
	copy_virt_memory(p);
	
	p->cpu_context.x19 = 0; 
	p->priority = current->priority;
	p->state = TASK_RUNNING;
	p->counter = p->priority;
	p->preempt_lock = 1; //disable preemtion until schedule_tail

	p->cpu_context.pc = (unsigned long)ret_from_fork;
	p->cpu_context.sp = (unsigned long)childregs;
	int pid = get_availible_pid();
	task[pid] = p;
	p->pid = pid;

	priorityQ_push(&runqueue,p->priority,p->pid);	
	preempt_enable();
	return pid;
}


struct trapframe* get_task_trapframe(struct task_struct *task){
	 //a small area (pt_regs area) was reserved at the top of the stack of the newly created task.
	unsigned long p = (unsigned long)task + THREAD_SIZE - sizeof(struct trapframe);
	return (struct trapframe *)p;
}

int do_exec(unsigned long start, unsigned long size, unsigned long pc)
{
	struct trapframe *regs = get_task_trapframe(current);
	memzero((unsigned long)regs, sizeof(*regs));
	
	regs->elr_el1 = pc;             // copy to elr_el1 
	regs->spsr_el1 = 0x00000000; // copy to spsr_el1 for enter el0 
	regs->sp =  0x0000ffffffffe000; 
	
	unsigned long code_page = allocate_user_page(0,current,pc);
//	unsigned long stack = allocate_user_page(current,regs->sp-8);
	if (!code_page) 
		return -1;
	// For stack, only map it and allocate when page fault
	// Since stack grow down, map from 0x0000ffffffffd000
	mmap((void *)0x0000ffffffffd000, PAGE_SIZE, PROT_READ|PROT_WRITE, \
			MAP_ANONYMOUS, NULL, 0);	
	
	memcpy((void *)code_page,(void *)start,size);
	//dump_mem((void *)code_page,size);
	unsigned long user_pgd = current->mm.pgd; 
	set_pgd(user_pgd);
	return 0;
}
