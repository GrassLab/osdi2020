#include "include/scheduler.h"
#include "include/queue.h"
#include "include/irq.h"
#include "include/string.h"
#include "include/mm.h"
#include "include/utils.h"
#include "include/fork.h"

static struct task_struct idle_task = IDLE_TASK;
struct task_struct * task[NR_TASKS] = {&(idle_task), };

void preempt_disable(void){
	current->preempt_lock++;
}

void preempt_enable(void){
	current->preempt_lock--;
}

void _schedule(void)
{       // Note that you are in El1 in scheduler
	preempt_disable();
	enable_irq();

	int next;
	int flag;

	struct task_struct * p;
	while (1) {
		next = 0;
		flag = 0;
		
		if(runqueue.heap_size>0){ //not empty queue
			next = priorityQ_pop(&runqueue);
			p = task[next];
		  	if (p && p->state == TASK_RUNNING && p->counter>0){
				priorityQ_push(&runqueue,p->priority,next);
				break;
			}
			// Else: a not running state task in queue...?
			// That might be something wrong
		}
			
		// If runQ empty, give exist task timeslice and keep waiting
		else {
			//give timeslice except pid 0
			for (int i = 1; i < NR_TASKS; i++) {
				p = task[i];
				if (p && p->state==TASK_RUNNING) {
					flag = 1;
					p->counter = p->priority;
					priorityQ_push(&runqueue,p->priority,i);
				}
			}

			if(flag == 1){
				continue;
			}
			// No running state task, then just switch to any IDLE task
			else{
				next = 0; 
				break;
			}
		}
	}
	//printf("Next: 0x%x\r\n",next);	
	
	context_switch(task[next]);
	
	disable_irq();
	preempt_enable();
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
	set_pgd(next->mm.pgd); //set page table for next task
	switch_to(prev, next);
}

void schedule_tail(void) {
	preempt_enable();
}

void timer_tick(){
	--current->counter;
	if (current->counter>0 || current->preempt_lock >0) {
		return;
	}
	// If counter <=0, it means reschedule flag set.
	current->counter=0;
	//printf("\r\n### No timeslice fot pid %d, reschedule\r\n",current->pid);
	
	_schedule();
}

void exit_process(){	
	preempt_disable();
	enable_irq();

	for (int i = 0; i < NR_TASKS; i++){
		if (task[i] == current) {
			task[i]->state = TASK_ZOMBIE;
			break;
		}
	}

	//reclaim user task page
	for(int i=0;i<current->mm.user_pages_count;i++){
		free_page(current->mm.user_pages[i].phy_addr);
		//printf("Free pid: %d, phy: %x/vir: %x\r\n",current->pid, current->mm.user_pages[i].phy_addr,current->mm.user_pages[i].vir_addr);
	}

	//reclaim page table	
	for(int i=0;i<current->mm.kernel_pages_count;i++){
		free_page(current->mm.kernel_pages[i]);
		//printf("Free pid: %d, phy: %x\r\n",current->pid, current->mm.kernel_pages[i]);
	}

	printf(">>> Done task %d, now exit\r\n",current->pid);
	
	disable_irq();	
	preempt_enable();
	
	schedule();
}
