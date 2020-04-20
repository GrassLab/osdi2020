#include "include/scheduler.h"
#include "include/queue.h"
#include "include/irq.h"
#include "include/string.h"
#include "include/mm.h"

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
			
		// If runQ empty, give exist task timeslice and keep waiting
		else {
			for (int i = 0; i < NR_TASKS; i++) {
				p = task[i];
				if (p) {
					flag = 1;
					p->counter = p->priority;
				// push a running state task back to queue
				// It might be push in other place, but now I just leave it here 
					if(p->state==TASK_RUNNING)
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
	char buffer[4];
	uart_send_string("Task pid: "); 
	itos(current->pid,buffer,10);
	uart_send_string(buffer);
	uart_send_string(" reschedule\r\n");
	
	enable_irq(); //have to enable irq in schedule state!
	_schedule();
	disable_irq();
}

void exit_process(){
	preempt_disable();
	for (int i = 0; i < NR_TASKS; i++){
		if (task[i] == current) {
			task[i]->state = TASK_ZOMBIE;
			break;
		}
	}

	if (current->stack) {
		free_page(current->stack);
	}
	preempt_enable();
	schedule();
}
