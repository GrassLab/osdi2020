#include "include/fork.h"
#include "include/mm.h"
#include "include/entry.h"
#include "include/scheduler.h"
#include "include/queue.h"

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

int privilege_task_create(void(* fn)){
	preempt_disable();
	struct task_struct *p;

	p = (struct task_struct *) get_free_page();
	if (!p)
		return 1;

	struct trapframe *childregs = get_task_trapframe(p);
	memzero((unsigned long)childregs, sizeof(struct trapframe));
	memzero((unsigned long)&p->cpu_context, sizeof(struct cpu_context));
	
	p->cpu_context.x19 = (unsigned long)fn;
	
	p->state = TASK_RUNNING;
	p->priority = 1;
	p->counter = 1;
	p->preempt_lock = 1;

	p->cpu_context.pc = (unsigned long)ret_from_fork;
	p->cpu_context.sp = (unsigned long)childregs; 
	int pid = get_availible_pid();
	task[pid] = p;	
	p->pid = pid;

	runQ_push(runQ,&runQ_tail,pid);
	preempt_enable();
	return pid;
}

int user_task_create()
{
	preempt_disable();
	struct task_struct *p;

	p = (struct task_struct *) get_free_page();
	if (!p) {
		return -1;
	}

	struct trapframe *childregs = get_task_trapframe(p);
	memzero((unsigned long)childregs, sizeof(struct trapframe));
	memzero((unsigned long)&p->cpu_context, sizeof(struct cpu_context));
	
	
	struct trapframe * cur_regs = get_task_trapframe(current);
	*childregs = *cur_regs; //copy content of parent register
	childregs->regs[0] = 0; //x0 in the new state is set to 0, because x0 will be interpreted by the caller as a return value of the syscall.
	
	p->cpu_context.x19 = 0; // set this for return to user in entry.S for safety		
	p->priority = current->priority;
	p->state = TASK_RUNNING;
	p->counter = p->priority;
	p->preempt_lock = 1; //disable preemtion until schedule_tail

	p->cpu_context.pc = (unsigned long)ret_from_fork;
	p->cpu_context.sp = (unsigned long)childregs;
	int pid = get_availible_pid();
	task[pid] = p;
	p->pid = pid;

	runQ_push(runQ,&runQ_tail,pid);
	preempt_enable();
	return pid;
}


struct trapframe* get_task_trapframe(struct task_struct *task){
	 //a small area (pt_regs area) was reserved at the top of the stack of the newly created task.
	unsigned long p = (unsigned long)task + THREAD_SIZE - sizeof(struct trapframe);
	return (struct trapframe *)p;
}

int do_exec(void(*func))
{
	struct trapframe *regs = get_task_trapframe(current);
	memzero((unsigned long)regs, sizeof(*regs));

	regs->elr_el1 = (unsigned long)func;             // copy to elr_el1 
	regs->spsr_el1 = 0x00000000; // copy to spsr_el1 for enter el0 
	
	unsigned long stack = get_free_page(); //allocate new user stack
	if (!stack) {
		return -1;
	}
	regs->sp = stack + PAGE_SIZE;

	current->stack = stack;
	return 0;
}
