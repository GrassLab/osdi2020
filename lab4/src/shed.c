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
  }
}

void idle(){
  while(1){
    schedule();
    delay(100000000);
  }
}
void init_init_task(void (*func)()){
    current->taskid = 0;
    current->cpu_context.x19 = (unsigned long)func;
    current->cpu_context.pc = (unsigned long)ret_from_fork;
    current->cpu_context.sp = (unsigned long)current + THREAD_SIZE;
}
void privilege_task_create(void (*func)()){
    struct task_struct *new_task = (struct task_struct *) get_free_page();
    new_task->taskid = n_tasks;
    new_task->counter = 3;
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
    int next, c;
    // while(1){
    c = 0;
    next = 0;
    for(int i=1;i<n_tasks;i++){
        if(task[i]&&task[i]->counter>c){
            c = task[i]->counter;
            next=i;
        }
    }
        // if(c)break;

    // }
    // int next;
    // next = ((current->taskid)+1)%n_tasks;
    
    switch_to(task[next]);
    
}

int N = 3;
void create_foo(){
    for(int i = 1; i <= N; ++i) { // N should > 2
        privilege_task_create(foo);
    }
}
void update_task_counter(){
    current->counter--;
    // uart_puts("current counter: \n");
    // uart_send_int(current->counter);
    // uart_send('\n');

    if(current->counter>0)
        return;
    current->counter=0;
    enable_irq();
    schedule();
    disable_irq();
}