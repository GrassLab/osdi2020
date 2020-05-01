#include "sched.h"
#include "thread.h"
#include "config.h"

extern task_t* current;
extern task_manager_t TaskManager;
task_t* task_run_queue[10];

void _schedule(void)
{
    printf("scheduling!\n");
    int next = 1;
    int i = 0;
    for(i = 1; i < TaskManager.task_num; i++){
        task_t *task = &TaskManager.task_pool[i];
        if(task->state == THREAD_RUNNABLE && task->counter > 0){
            next = i;
            break;
        }
    }
    if(i == TaskManager.task_num) { next = 0; }
    printf("next is %d!\n", next);
    task_t* new_task = &TaskManager.task_pool[next]; 
	switch_to(new_task);
}

void switch_to(task_t * next) 
{
	if (current == next){
        printf("the same task!\n");
		return;
    }
    printf("first switch the init task!\n");
	struct task_struct * prev = current;
	current = next;
	cpu_switch_to(prev, next);
}
