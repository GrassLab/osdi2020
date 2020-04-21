#include "../include/uart.h"
#include "../include/time.h"
#include "../include/task.h"


#define INIT_TASK \
/*cpu_context*/	{ {0,0,0,0,0,0,0,0,0,0,0,0,0}, \
/* state etc */	0,0,1,0,0 \
}

struct task init_task = INIT_TASK;

extern struct task* get_current();
extern void set_current(struct task* task_struct);
// struct task *current = &(init_task);


extern void switch_to(struct task* prev, struct task* next);
extern void ret_from_fork();

void task_manager_init()
{
    // uart_memset(TaskManager.kstack_pool, '\0', 64*4096);
    set_current(&(init_task));
    TaskManager.task_num = 0;
}

void privilege_task_create(void(*func)())
{
    // assign the task id
    unsigned int task_id = TaskManager.task_num;

    struct task* new_task = &TaskManager.task_pool[task_id]; 
    uart_hex((unsigned long) new_task);
    new_task->cpu_context.x19 = (unsigned long) func;
    uart_hex((unsigned long) func);
    new_task->cpu_context.pc = (unsigned long)ret_from_fork;
    new_task->cpu_context.sp = (unsigned long) &TaskManager.kstack_pool[task_id];
    new_task->task_id = task_id;
    TaskManager.task_num++;
    // put the task into the runqueue
}


void context_switch(struct task* next)
{
    struct task* current = get_current();
    struct task* prev = current;
    switch_to(prev, next);
}

void schedule()
{
    // int idx = 0;
    // while (1) {
    //     struct task* next = &TaskManager.task_pool[idx];
    //     context_switch(next);
    //     idx = (idx+1) % TaskManager.task_num;
    // }

    // struct task* task0 = &TaskManager.task_pool[0];
    // struct task* task1 = &TaskManager.task_pool[1];
    // context_switch(task0);
    // context_switch(task1);


    for(int i = 0; i < N; ++i) { // N should > 2
        struct task* next = &TaskManager.task_pool[i];
        context_switch(next);
    }
    
    return;
}


/* 
 ** porcess for test
 */
void foo()
{
    while(1) {
        struct task* current = get_current();
        uart_puts("Task_id: ");
        uart_hex(current->task_id);
        uart_puts("\n");
            
        // delay(1000000);
        wait_cycles(1000000);
        schedule();
            // idle();
    }
}

void idle()
{
    while(1){
        schedule();
        // delay(1000000);
        uart_puts("idle...\n");
        wait_cycles(1000000);
    }
}
