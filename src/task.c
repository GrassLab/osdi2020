#include "task.h"

int task_num = 0;
static struct task init_task = INIT_TASK;
struct task *current = &(init_task);
struct task *task_list[TASKS_POOL] = {&(init_task), };
char kstack_pool[64][4096];

void foo(){
  while(1) {
    uart_puts("Task id: ");
    uart_int(current -> task_id);
    uart_send('\n');
    delay(100000000);
    schedule();
  }
}

void idle(){
  while(1){
    delay(100000000);
    schedule();
  }
}

void privilege_task_create(void(*func)()) {
  struct task *new_task = (struct task_struct *) get_free_page();;
  new_task->task_id = task_num;
  new_task->counter = 0;
  new_task->cpu_context.x19 = (unsigned long)func;
  new_task->cpu_context.pc = (unsigned long)ret_from_fork;
  new_task->cpu_context.sp = (unsigned long)&kstack_pool[task_num];
  task_list[task_num] = new_task;

  uart_puts("Create new Task: ");
  uart_int(task_list[task_num]->task_id);
  uart_send('\n');
  task_num++;
}

void context_switch(struct task* next) {
  if (current == next)
	return;
  struct task* prev = current;
  current = next;
  /*
  uart_puts("\nprev id: ");
  uart_int(prev->task_id);
  uart_puts("\nnext id: ");
  uart_int(next->task_id);
  */
  switch_to(prev, next);
}

void schedule() {
  int next;
  next = ((current->task_id)+1) % task_num;
  context_switch(task_list[next]);
}

int N = 10;
void create_tasks() {
  for (int num = 0; num < N; num++)
    privilege_task_create(foo);
  idle();
}
