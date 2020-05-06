#include "tools.h"
#include "uart.h"
#include "mbox.h"
#include "peripherals.h"
#include "gpio.h"
#include "utils.h"
#include "lfb.h"
#include "printf.h"
#include "irq.h"
#include "timer.h"
#include "entry.h"
#include "multitask.h"

/*
 * MEMORY MANAGEMENT
 */
static unsigned short mem_map[PAGING_PAGES] = {0,};

unsigned long get_free_page(){

	for (int i = 0; i < PAGING_PAGES; i++){
		if (mem_map[i] == 0){
			mem_map[i] = 1;
			return LOW_MEMORY + i*PAGE_SIZE;
		}
	}
	return 0;
}

void free_page(unsigned long p){
	mem_map[(p - LOW_MEMORY) / PAGE_SIZE] = 0;
}


/*
 * TASK MANAGEMENT
 */

struct task_struct *task_pool[MAX_THREAD_NUM] = {0};
static int current_thread_number = 0;
static int schedule_turn = 0;
static int schedule_timer_count = 0;

void init(){
	while(1){
		schedule();
	}
}

int copy_process(unsigned long clone_flags, unsigned long fn, unsigned long arg, unsigned long free_old_stack){

	//preempt_disable();

	// get free page from memory for new process
	struct task_struct *new_task_struct = (struct task_struct *)get_free_page();
	if(new_task_struct == 0){	// memory allocation failed
		return 1;
	}
	struct user_context *new_user_context = (struct user_context *)( (unsigned long)new_task_struct + (unsigned long)THREAD_SIZE - (unsigned long)sizeof(struct user_context) );

	// get current task struct
	struct task_struct *current_task_struct = (struct task_struct *)task_pool[get_current()];
	struct user_context *current_user_context = (struct user_context *)( (unsigned long)current_task_struct + (unsigned long)THREAD_SIZE - (unsigned long)sizeof(struct user_context) );
	
	// get pid
	int pid = current_thread_number;
	current_thread_number++;
	task_pool[pid] = new_task_struct;
	
	// clean old stack if needed
	if(free_old_stack > 0){
		free_page(free_old_stack);
	}
	unsigned long stack = get_free_page();

	// *******************
	if(clone_flags == FLAG_KERNEL){
		new_task_struct->_cpu_context.x19 = fn;
		new_task_struct->_cpu_context.x20 = arg;
		new_task_struct->_cpu_context.sp = (unsigned long)new_user_context;
	}else if(clone_flags == FLAG_FORK){
		// copy the task_struct and stack of parent to child
		memcpy((void *)new_task_struct, (void *)current_task_struct, PAGE_SIZE);
		memcpy((void *)stack, (void *)current_task_struct->stack, PAGE_SIZE);
		
		*new_user_context = *current_user_context;
		new_user_context->regs[0] = 0;
		new_task_struct->stack = stack;

		new_task_struct->_cpu_context.sp = (unsigned long)new_task_struct + (unsigned long)current_task_struct->_cpu_context.sp - (unsigned long)current_task_struct;
		
		new_user_context->sp = stack + (unsigned long)current_user_context->sp - (unsigned long)current_task_struct->stack;

	}else{
		*new_user_context = *current_user_context;
		new_user_context->regs[0] = 0;
		new_user_context->sp = stack + PAGE_SIZE;
		new_task_struct->stack = stack;
		new_task_struct->_cpu_context.sp = (unsigned long)new_user_context;
	}
	// *******************

	// other settings
	new_task_struct->flags = clone_flags;
	new_task_struct->state = TASK_RUNNING;
	new_task_struct->counter = new_task_struct->priority;
	new_task_struct->preempt_count = 1;
	new_task_struct->_cpu_context.pc = (unsigned long)ret_from_fork;

	//preempt_enable();
	uart_puts("new process: ");
	uart_int(pid);
	uart_puts("\n");
	return pid;
}

int do_fork(){
	unsigned long stack = get_free_page();
	return copy_process(FLAG_FORK, 0, 0, stack);
}

int do_exec(unsigned long address){
	struct user_context * current_user_context = (struct user_context *)( (unsigned long)task_pool[get_current()] + (unsigned long)THREAD_SIZE - (unsigned long)sizeof(struct user_context) );
	current_user_context->pc = address;
	current_user_context->pstate = PSTATE_EL0t;
	unsigned long stack = get_free_page();
	current_user_context->sp = stack + PAGE_SIZE;
	task_pool[get_current()]->stack = stack;	
	return 0;
}

void exit_process(int pid){
	struct task_struct * spec_task_struct = task_pool[pid];
	for(int i=0; i<MAX_THREAD_NUM; i++){
		if(task_pool[i] == spec_task_struct){
			spec_task_struct->state = TASK_ZOMBIE;
			break;
		}
	}
	if(spec_task_struct->stack > 0){
		free_page(spec_task_struct->stack);
	}
	schedule();
}

void context_switch(int pid){
    
	struct task_struct * prev = task_pool[get_current()];
	struct task_struct * next = task_pool[pid];
    set_current(pid);

	switch_to(prev, next);
}

void schedule(){
	int pid;
	struct task_struct * tmp;
	while(1){
	/*	
		uart_puts("schedule turn: ");
		uart_int(schedule_turn);
		uart_puts("\n");
	*/
		// find a running task iteratively
		schedule_turn++;
		pid = schedule_turn % current_thread_number;
		tmp = task_pool[pid];
		if(tmp && (tmp->state == TASK_RUNNING) ){
			break;
		}
	}
/*
	uart_puts("context switch: ");
	uart_int(pid);
	uart_puts("\n");
*/
	context_switch(pid);
}

int check_reschedule(){
    if (schedule_timer_count > 5)
    {
        schedule_timer_count = 0;
        return 1;
    }
    else
        return 0;
}

void timer_tick(){
	if(schedule_timer_count > 5){
		//uart_puts("timer schedule\n");
		schedule_timer_count = 0;	
		enable_irq();
		schedule();
		disable_irq();
	}
	//uart_puts("timer not schedule\n");
}

void schedule_cnt_add(int num){
	schedule_timer_count += num;
}
