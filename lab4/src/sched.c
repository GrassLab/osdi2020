#include "sched.h"
#include "thread.h"
#include "config.h"

task_t* current;
extern task_manager_t TaskManager;

void _schedule(void)
{
    task_t* new_task = &TaskManager.task_pool[1]; 
	switch_to(new_task);
}

void schedule(void)
{
	_schedule();
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
