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

/* ======================= resource status =======================*/
unsigned long long task_allocate_status = 0;
unsigned long long user_stack_allocate_status = 0;

void SET(unsigned long long *status, int taskId){
	unsigned long long bit = 1;
	bit = bit << taskId;

	*status = *status | bit;
}

void CLR(unsigned long long *status, int taskId){
	unsigned long long bit = 1;
	bit = bit << taskId;

	*status = *status & (~bit);
}

int isSET(unsigned long long status, int taskId){
	unsigned long long bit = 1;
	bit = bit << taskId;

	if(status & bit) return 1;
	return 0;
}

int ALLOCATABLE(unsigned long long status){
	for(int id=0; id<64; id++){
		if( isSET(status, id)==0 ) return id;
	}

	return -1;
}

int BITFULL(unsigned long long status){
	if(status == 0xFFFFFFFFFFFFFFFF) return 1;
	return 0;
}



/* ======================= task =======================*/

int privilege_task_create( void(*func)() ){
	if( BITFULL(task_allocate_status) || BITFULL(user_stack_allocate_status) ) return -1;

	int cur_taskId = ALLOCATABLE(task_allocate_status);
	SET(&task_allocate_status, cur_taskId);

	int ustack_id = ALLOCATABLE(user_stack_allocate_status);
	SET(&user_stack_allocate_status, ustack_id);


	task_pcb_pool[cur_taskId].taskId = cur_taskId;
	task_pcb_pool[cur_taskId].context.lr = (unsigned long long)func;
	task_pcb_pool[cur_taskId].context.kstack = (unsigned long long)&kstack_pool[cur_taskId][4096];


	task_pcb_pool[cur_taskId].ustack_id = ustack_id;
	task_pcb_pool[cur_taskId].ustack = (unsigned long long)&ustack_pool[ustack_id][4096];


	for(int i=0; i<4096; i++) kstack_pool[cur_taskId][i] = '\0';
	for(int i=0; i<4096; i++) ustack_pool[ustack_id][i] = '\0';

	task_pcb_pool[cur_taskId].exitStatus = -1;
	task_pcb_pool[cur_taskId].pstatus = ready;

	enQueue(&runQueue, cur_taskId);	

	return cur_taskId;
}

void do_exec( void(*func)() ){
	task_t *curTask = get_cur_task();
	curTask->umode_lr = (unsigned long long)func;
	

	curTask->context.lr = (unsigned long long)func;
	curTask->context.kstack = (unsigned long long)&kstack_pool[curTask->taskId][4096];
	curTask->ustack = (unsigned long long)&ustack_pool[curTask->ustack_id][4096];

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
	if(curTask->pstatus == ready) enQueue(&runQueue, curTask->taskId);
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
			uart_puts("i'm idle");
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

void leave_fork(){
	kernel_routine_exit();

	/*
		using sp to restore register
		make sure go to this function using b instr. but not bl instr. 
		because bl instr. will change the sp, it will make function restore error value to register and cause corruption.

		Question:
		How to force compiler using b instr. but not bl instr. in C language.
	*/
	restore_all_reg();
}

void do_fork(){
	task_t *curTask = get_cur_task();

	int child = privilege_task_create( leave_fork );
	int parent = curTask->taskId;

	int child_ustack_id = task_pcb_pool[child].ustack_id;
	int parent_ustack_id = curTask->ustack_id;

	int offset;
	unsigned long long fp;
	unsigned long long *argu = (unsigned long long*)(curTask->context.kstack - 32 * 8);
	
	fp = argu[29];
	offset = fp - (unsigned long long)&ustack_pool[parent_ustack_id][0];
	argu[29] = (unsigned long long)&ustack_pool[child_ustack_id][offset];

	argu[0] = 0;

	for(int i=0; i<4096; i++) kstack_pool[child][i] = kstack_pool[parent][i];
	for(int i=0; i<4096; i++) ustack_pool[child_ustack_id][i] = ustack_pool[parent_ustack_id][i];
	argu[0] = child;
	argu[29] = fp;

	//user
	task_pcb_pool[child].umode_lr = curTask->umode_lr;
	offset = curTask->ustack - (unsigned long long)&ustack_pool[parent_ustack_id][0];
	task_pcb_pool[child].ustack = (unsigned long long)&ustack_pool[child_ustack_id][offset];

	//kernel
	offset = curTask->context.kstack - 32 * 8 - (unsigned long long)&kstack_pool[parent][0];
	task_pcb_pool[child].context.kstack  = (unsigned long long)&kstack_pool[child][offset];

	// uart_puts("fork: parent id: ");
	// uart_hex(parent);
	// uart_puts("   ->  user stack addr ");
	// uart_hex(&ustack_pool[parent_ustack_id][0]);
	// uart_puts(" ~ ");
	// uart_hex(&ustack_pool[parent_ustack_id][4095]);
	// uart_puts("   ->  sp  ");
	// uart_hex((unsigned long long)curTask->ustack);
	// uart_puts("\n");

	// uart_puts("fork: child id: ");
	// uart_hex(child);
	// uart_puts("   ->  user stack ");
	// uart_hex(&ustack_pool[child_ustack_id][0]);
	// uart_puts(" ~  ");
	// uart_hex(&ustack_pool[child_ustack_id][4095]);
	// uart_puts("   ->  sp  ");
	// uart_hex((unsigned long long)task_pcb_pool[child].ustack);
	// uart_puts("\n\n");
}

void do_exit(int status){
	task_t *curTask = get_cur_task();

	curTask->pstatus = zombie;
	curTask->exitStatus = status;

	//release user stack
	int ustack_id = curTask->ustack_id;
	CLR(&user_stack_allocate_status, ustack_id);

	ReSchedule = 1;
	schedule();
}

void zombieReaper(){
	while(1){
		for(int id=0; id<64; id++){
			if(isSET(task_allocate_status, id)) {
				if( task_pcb_pool[id].pstatus == zombie ) CLR(&task_allocate_status, id);	
			}
		}

		ReSchedule = 1;
		schedule();
	}
}