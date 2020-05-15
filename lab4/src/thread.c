#include "thread.h"
#include "config.h"

task_manager_t TaskManager;
task_t* current;
extern schedule();

void idle_task(){
    while(1){
        printf("idle....\n");
        // delay(10000000);
        delay(100000);
        schedule();
    }
}

void create_idle_task(){
    task_t* init_task = privilege_task_create((unsigned long)&idle_task);
    printf("idle task current id is: %d\n", init_task->task_id);
}

void init_task_manager(){
    TaskManager.task_num = 0;
    for(int i = 0; i < MAX_TASK_SIZE; i++){
        TaskManager.task_pool[i].task_id = i;
        TaskManager.task_pool[i].state = ZOMBIE;
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
    task_t *task = get_current();
    unsigned int el_level;
    asm volatile ("mrs %0, CurrentEL" : "=r" (el_level));
    printf("now level is: %d\n", el_level);
    printf("exec--- id is: %d\n", task->task_id);
    task->mode = USER_MODE;
    unsigned long user_stack = current->user_context.sp_el0;
    unsigned long user_cpu_state = 0x0;
    asm volatile("msr sp_el0, %0" :: "r" (user_stack));
    asm volatile("msr spsr_el1, %0" :: "r" (user_cpu_state));
    asm volatile("msr elr_el1, %0" :: "r" (func));
    asm volatile("eret");
}

int do_fork(){
    printf("fork -----------------------------------\n");
    int child_taskId = -1;
    for(int i = 0; i < 64; i++) {
        if (TaskManager.task_pool[child_taskId].state == ZOMBIE) {
            break;
        }
        child_taskId = (child_taskId + i) % 64;
    }
    task_t *parent = get_current();
    task_t *child = &TaskManager.task_pool[child_taskId];
    child->task_id = child_taskId;
    child->parent_id = parent->task_id;
    _copy_stack(TaskManager.kstack_pool[parent->task_id], TaskManager.kstack_pool[child_taskId], 4096);
    _copy_stack(TaskManager.ustack_pool[parent->task_id], TaskManager.ustack_pool[child_taskId], 4096);

    child->cpu_context.x19 = parent->cpu_context.x19;
    child->cpu_context.x20 = parent->cpu_context.x20;
    child->cpu_context.x21 = parent->cpu_context.x21;
    child->cpu_context.x22 = parent->cpu_context.x22;
    child->cpu_context.x23 = parent->cpu_context.x23;
    child->cpu_context.x24 = parent->cpu_context.x24;
    child->cpu_context.x25 = parent->cpu_context.x25;
    child->cpu_context.x26 = parent->cpu_context.x26;
    child->cpu_context.x27 = parent->cpu_context.x27;
    child->cpu_context.x28 = parent->cpu_context.x28;

    Trapframe *trapframe = parent->trapframe;
    unsigned long ustack_offset = ((unsigned long) &TaskManager.ustack_pool[parent->task_id])
                                - (parent->user_context.sp_el0);
    unsigned long fp_offset = ((unsigned long) &TaskManager.ustack_pool[parent->task_id])
                                - trapframe->regs[29];
    unsigned long trapframe_offset = ((unsigned long) &TaskManager.kstack_pool[parent->task_id]) 
                                - parent->trapframe;

    // unsigned long ustack_offset = ((unsigned long) &TaskManager.ustack_pool[current->task_id]) - kstack_regs->sp_el0;
    // unsigned long trapframe_offset = ((unsigned long) &TaskManager.kstack_pool[current->task_id]) - current->trapframe;
    // unsigned long fp_offset = ((unsigned long) &TaskManager.ustack_pool[current->task_id]) - kstack_regs->regs[29];

    child->trapframe = (unsigned long) &TaskManager.kstack_pool[child->task_id] - trapframe_offset;

    child->user_context.sp_el0 = (unsigned long) &TaskManager.ustack_pool[child->task_id] - ustack_offset;
    child->user_context.spsr_el1 = parent->user_context.spsr_el1;
    child->user_context.elr_el1 = parent->user_context.elr_el1;
    
    child->cpu_context.fp = (unsigned long) &TaskManager.ustack_pool[child->task_id] - fp_offset;
    child->cpu_context.sp = (unsigned long) &TaskManager.kstack_pool[child->task_id] - trapframe_offset;
    child->cpu_context.pc = (unsigned long) fork_child_exit;

    printf("pc location is: %x\n", fork_child_exit);

    child->state = THREAD_RUNNABLE;
    child->mode = KERNEL_MODE;
    child->counter = 0;
    child->rescheduled = 0;
    TaskManager.task_num++;

    printf("new child id is: %d\n",child->task_id);
    printf("new child state is: %d\n",child->state);
    printf("current total task num: %d\n",TaskManager.task_num);

    return child->task_id;

}

extern struct task* get_current();

// void context_switch(struct task* next){
//     struct task* prev = current;
//     current = next;
//     cpu_switch_to(prev, next);
// }

void schedule_tail(void) 
{
	preempt_enable();
}

void do_exit()
{
    printf("In the exit, current id is %d\n", current->task_id);
    current->state = ZOMBIE;
    TaskManager.task_num--;
    schedule();
}

void preempt_disable(void)
{
	// current->preempt_count++;
}

void preempt_enable(void)
{
	// current->preempt_count--;
}