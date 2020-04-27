#include "task.h"
#include "uart.h"
#include "irq.h"

task_t task_pcb_pool[64];
char kstack_pool[64][4096];
char ustack_pool[64][4096];


int new_taskId = 0;
int ReSchedule = 0;
queue_t runQueue;
task_t *idle_pcb = &task_pcb_pool[0];

/* ======================= queue =======================*/
void init_Queue(queue_t *q){
	q->front = 0;
	q->rear = 0;
	q->isFull = 0;
}

int isEmpty(queue_t *q){
	return (q->front==q->rear && q->isFull==0);
}

void enQueue(queue_t *q, int num){
	if(q->isFull) {
		uart_puts("Queue is full! \n");
		return;
	}

	q->rear = (q->rear+1)%MAX_QUEUE;
	q->circular_queue[q->rear] = num;
	if(q->front == q->rear) q->isFull=1;
}

int deQueue(queue_t *q){
	if(isEmpty(q)){
		uart_puts("Queue is empty! \n");
		return -1;
	}

	
	q->front = (q->front+1)%MAX_QUEUE;
	q->isFull=0;

	return q->circular_queue[q->front];

}

/* ======================= task =======================*/

void privilege_task_create( void(*func)() ){
	int cur_taskId = new_taskId;

	task_pcb_pool[cur_taskId].taskId = cur_taskId;

	task_pcb_pool[cur_taskId].context.lr = (unsigned long long)func;
	task_pcb_pool[cur_taskId].context.kstack = (unsigned long long)&kstack_pool[cur_taskId][4095];

	task_pcb_pool[cur_taskId].ustack = (unsigned long long)&ustack_pool[cur_taskId][4095];

	for(int i=0; i<4096; i++) kstack_pool[cur_taskId][i] = '\0';
	for(int i=0; i<4096; i++) ustack_pool[cur_taskId][i] = '\0';

	enQueue(&runQueue, cur_taskId);
	new_taskId++;
}

void do_exec( void(*func)() ){
	task_t *curTask = get_cur_task();
	curTask->umode_lr = (unsigned long long)func;

	asm volatile(
		"msr     elr_el1, %[retAddr];"
		"msr     sp_el0, %[ustack];"
		"eret;"
		:
		: [retAddr] "r" (func), [ustack] "r" ( curTask->ustack )
		:
	);
}

void context_switch(int next_taskId){
	task_t *curTask = get_cur_task();
	enQueue(&runQueue, curTask->taskId);
	set_cur_task( &task_pcb_pool[next_taskId] );
	switch_to(&curTask->context, &task_pcb_pool[next_taskId].context);
}

void schedule(){
	if(ReSchedule){
		ReSchedule = 0;
		int next_taskId = deQueue(&runQueue);
		context_switch(next_taskId);
	}
}

//idle task
void idle(){
	while(1){
		int next_taskId = deQueue(&runQueue);
		if(next_taskId != -1){
			set_cur_task( &task_pcb_pool[next_taskId] );
			switch_to(&task_pcb_pool[0].context, &task_pcb_pool[next_taskId].context);
		}else{
			sleep();
		}
	}
}

void kernel_routine_entry(){
	task_t *curTask = get_cur_task();
	
	store_umode_lr_sp(&curTask->umode_lr, &curTask->ustack);

}

void kernel_routine_exit(){
	task_t *curTask = get_cur_task();

	restore_umode_lr_sp(curTask->umode_lr, curTask->ustack);

}
