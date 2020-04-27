#include "tools.h"
#include "mm.h"
#include "exception.h"

static struct task_struct init_task = INIT_TASK;
struct task_struct *current = &(init_task);
struct task_struct * task[NR_TASKS] = {&(init_task), };
int n_tasks = 1;



void foo(){
  while(1) {
    uart_puts("Task id: ");
    uart_send_int(current -> taskid);
    uart_send('\n');
    delay(100000000);
    schedule();
  }
}

void idle(){
  while(1){
    schedule();
    delay(100000000);
  }
}
void privilege_task_create(void (*func)()){
    struct task_struct *new_task = (struct task_struct *) get_free_page();
    new_task->taskid = n_tasks;
    new_task->cpu_context.x19 = (unsigned long)func;
    new_task->cpu_context.pc = (unsigned long)ret_from_fork;
    new_task->cpu_context.sp = (unsigned long)new_task + THREAD_SIZE;
    task[n_tasks] = (new_task);
    n_tasks++;
    uart_puts("Create new Task: \n");
    uart_send_int(new_task->taskid);
    uart_send('\n');

}
void switch_to(struct task_struct *next) 
{
	if (current == next) 
		return;
	struct task_struct * prev = current;
	current = next; 
	cpu_switch_to(prev, next);

}
void schedule(){
    int next;
    next = ((current->taskid)+1)%n_tasks;
    switch_to(task[next]);
}

int N = 10;
void create_foo(){
    for(int i = 1; i <= N; ++i) { // N should > 2
        privilege_task_create(foo);
    }
    idle();
}