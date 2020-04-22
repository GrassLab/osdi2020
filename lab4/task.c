#include "task.h"
#include "uart.h"
#include "irq.h"

task_t task_pcb_pool[64];
char kstack_pool[64][4096];
int new_taskId=0;

void privilege_task_create( void(*func)() ){
	int cur_taskId = new_taskId;

	task_pcb_pool[cur_taskId].taskId = cur_taskId;
	task_pcb_pool[cur_taskId].context.lr = (unsigned long long *)func;
	task_pcb_pool[cur_taskId].context.sp = (unsigned long long *)&kstack_pool[cur_taskId][0];

	for(int i=0; i<4096; i++) kstack_pool[cur_taskId][i] = '\0';

	new_taskId++;
}

void context_switch(int next_taskId){
	int cur_taskId = get_cur_taskId();
	set_cur_taskId(next_taskId);
	switch_to(&task_pcb_pool[cur_taskId].context, &task_pcb_pool[next_taskId].context);
}

void task1(){
	while(1){
		uart_puts("1...\n");
		sleep();
		context_switch(1);
	}
}

void task2(){
	while(1){
		uart_puts("2...\n");
		sleep();
		context_switch(0);
	}
}

void test(){
	privilege_task_create(task1);
	privilege_task_create(task2);

	set_cur_taskId(0);
	task1();
}