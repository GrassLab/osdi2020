#include "include/scheduler.h"
#include "include/queue.h"
#include "include/irq.h"

static struct task_struct init_task = IDLE_TASK;
struct task_struct *current = &(init_task);
struct task_struct * task[NR_TASKS] = {&(init_task), };
int nr_tasks = 1;

void preempt_disable(void){
	current->preempt_lock++;
}

void preempt_enable(void){
	current->preempt_lock--;
}

void _schedule(void)
{
	preempt_disable();

	int next;
	int flag;

	struct task_struct * p;
	while (1) {
		next = 0;
		flag = 0;
		
		if(!isempty(runQ_head,runQ_tail)){
			next = runQ_pop(runQ,&runQ_head);
			p = task[next];
		  	if (p && p->state == TASK_RUNNING && p->counter>0){
				runQ_push(runQ,&runQ_tail,next);
				break;
			}
			// Else: a not running state task in queue...?
			// That might be something wrong
		}
			
		// If runQ empty, give those runnable task timeslice
		else {
			for (int i = 0; i < NR_TASKS; i++) {
				p = task[i];
				if (p && p->state==TASK_RUNNING) {
					flag = 1;
					// set timeslice and put back to queue
					p->counter = p->priority;
					runQ_push(runQ,&runQ_tail,i);
				}
			}

			if(flag == 1)
				continue;
			// No running state task, then just switch to any IDLE task
			else{
				next = 0; 
				break;
			}
		}
	}
	context_switch(task[next]);
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
	current = next;
	switch_to(prev, next);
}


void init_runQ(){
	runQ_head = 0;
	runQ_tail = 0;
}

void schedule_tail(void) {
	preempt_enable();
}

void timer_tick(){
	--current->counter;

	if (current->counter>0 || current->preempt_lock >0) {
		return;
	}
	current->counter=0;
	uart_send_string("Task pid: "); 
	uart_hex(current->pid);
	uart_send_string(" reschedule\r\n");
	
	enable_irq();
	_schedule();
	disable_irq();
}
