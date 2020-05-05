#include "thread.h"
#include "config.h"

task_manager_t TaskManager;
task_t* current;
extern schedule();

void idle_task(){
    while(1){
        printf("idle....\n");
        delay(10000000);
        schedule();
    }
}

void create_idle_task(){
    task_t* init_task = privilege_task_create((unsigned long)&idle_task);
}

void init_task_manager(){
    TaskManager.task_num = 0;
    for(int i = 0; i < MAX_TASK_SIZE; i++){
        TaskManager.task_pool[i].task_id = i;
    }
    create_idle_task();
}

task_t* privilege_task_create(unsigned long fn){
    int task_id = TaskManager.task_num;
    printf("task id %d create\n", task_id);

    task_t* new_task = &TaskManager.task_pool[task_id]; 
    _memset(TaskManager.kstack_pool[task_id],'\0', STACK_SIZE);
    _memset(TaskManager.ustack_pool[task_id],'\0', STACK_SIZE);

    new_task->counter = 1;
    new_task->state = THREAD_RUNNABLE;

    new_task->cpu_context.x19 = (unsigned long) fn;
    new_task->cpu_context.x20 = 0;

    new_task->user_context.sp_el0 = &TaskManager.ustack_pool[task_id];
    new_task->user_context.spsr_el1 = 0;
    new_task->user_context.elr_el1 = 0;
    new_task->trapframe = 0;
    new_task->mode = KERNEL_MODE;
    
    new_task->cpu_context.pc = (unsigned long)ret_from_fork;
    new_task->cpu_context.sp = (unsigned long) &TaskManager.kstack_pool[task_id];
    
    TaskManager.task_num++;

    return new_task;
}

void do_exec(void(*func)()){
    if(current->rescheduled){
        current->rescheduled = 0;
        schedule();
    }
    if(current->mode == KERNEL_MODE){
        unsigned long user_stack = current->user_context.sp_el0;
        unsigned long user_cpu_state = 0x0;
        asm volatile("msr sp_el0, %0" :: "r" (user_stack));
        asm volatile("msr spsr_el1, %0" :: "r" (user_cpu_state));
        asm volatile("msr elr_el1, %0" :: "r" (func));
        asm volatile("eret");
        current->mode = USER_MODE;
    }
    else if(current->mode == USER_MODE){
        
    }

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