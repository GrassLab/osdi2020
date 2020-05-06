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
    int max_c = -1;
    for(i = 1; i < TaskManager.task_num; i++){
        task_t *task = &TaskManager.task_pool[i];
        if(task->state == THREAD_RUNNABLE && task->counter > max_c){
            next = i;
            max_c = task->counter;
        }
    }
    if(max_c == 0){
        for(i = 1; i < TaskManager.task_num; i++){
            task_t *task = &TaskManager.task_pool[i];
            task->counter = 1;
        }
    }
    // if(i == TaskManager.task_num) { next = 0; }
    if(TaskManager.task_num == 1) { next = 0;}
    task_t* new_task = &TaskManager.task_pool[next]; 
    printf("next is %d!\n", next);
    printf("total thread nunmber is %d!\n", TaskManager.task_num);
	switch_to(new_task);
}

void switch_to(task_t * next) 
{
	if (current == next){
        printf("the same task!\n");
		return;
    }
	struct task_struct * prev = current;
	current = next;
	cpu_switch_to(prev, next);
    printf("switch finished\n");

}

void check_reschedule(){
    printf("check reschedule!!!\n");
    printf("now task id is %d\n", current->task_id);

    if(current->rescheduled){
        printf("reschedul!!!\n");
        schedule();
    }
}