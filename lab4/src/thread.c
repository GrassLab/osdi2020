#include "thread.h"

task_manager_t TaskManager;
task_t* current;

void init_task_manager(){
    for(int i = 0; i < MAX_TASK_SIZE; i++){
        TaskManager.task_pool[i].task_id = i;
    }
    task_t* init_task = privilege_task_create(0, 0);
    current = init_task;
}

task_t* privilege_task_create(unsigned long fn, int k){
    int task_id = k;
    task_t* new_task = &TaskManager.task_pool[task_id]; 
    _memset(TaskManager.kstack_pool[task_id],'\0');
    _memset(TaskManager.ustack_pool[task_id],'\0');
    new_task->cpu_context.x19 = (unsigned long) fn;
    new_task->cpu_context.x20 = 0;

    new_task->user_context.sp_el0 = &TaskManager.ustack_pool[task_id];
    new_task->user_context.spsr_el1 = 0;
    new_task->user_context.elr_el1 = 0;
    new_task->trapframe = 0;
    
    new_task->cpu_context.pc = (unsigned long)ret_from_fork;
    new_task->cpu_context.sp = (unsigned long) &TaskManager.kstack_pool[task_id];

    return new_task;
}

extern struct task* get_current();

void context_switch(struct task* next){
    struct task* prev = current;

    current = next;
    
    cpu_switch_to(prev, next);
}

void schedule_tail(void) 
{
	preempt_enable();
}

void preempt_disable(void)
{
	// current->preempt_count++;
}

void preempt_enable(void)
{
	// current->preempt_count--;
}