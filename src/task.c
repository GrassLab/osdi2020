#include "../include/uart.h"
#include "../include/time.h"
#include "../include/task.h"
#include "../include/queue.h"



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
    // uart_hex((unsigned long) new_task);
    new_task->cpu_context.x19 = (unsigned long) func;
    new_task->cpu_context.x20 = task_id;
    // uart_hex((unsigned long) func);
    new_task->cpu_context.pc = (unsigned long)ret_from_fork;
    new_task->cpu_context.sp = (unsigned long) &TaskManager.kstack_pool[task_id];
    new_task->task_id = task_id;
    new_task->reschedule_flag = 0;
    TaskManager.task_num++;

    // put the task into the runqueue
    // QUEUE_SET(RunQueue, (unsigned long) new_task);
    // QUEUE_PUSH(RunQueue);
}


void context_switch(struct task* next)
{
    struct task* current = get_current();
    struct task* prev = current;
    switch_to(prev, next);
}

void schedule()
{
    for(int i = 0; i < N; ++i) { // N should > 2
        struct task* next = &TaskManager.task_pool[i];
        next->counter = 3;
        context_switch(next);
    }
    // while (!QUEUE_EMPTY(RunQueue)) {
    //     struct task* next = QUEUE_GET(RunQueue);
    //     QUEUE_POP(RunQueue);

    //     QUEUE_SET(RunQueue, (unsigned long) next);
    //     QUEUE_PUSH(RunQueue);

    //     context_switch(next);
    // }

    return;
}


/* 
 ** porcess for test
 */
void foo(unsigned int task_id)
{
    while(1) {
        struct task* current = get_current();
        uart_puts("Task_id: ");
        // uart_hex(current->task_id);
        uart_hex(task_id);
        uart_puts("\n");
            
        // delay(1000000);
        wait_cycles(1000000);
        if (current->reschedule_flag == 1) {
            uart_puts("reschedule...\n");
            current->reschedule_flag = 0;
            schedule();
        }
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
