#include "task.h"

//static struct task init_task = INIT_TASK;
struct task *current;// = &(init_task);
struct task *task_list[TASKS_POOL];// = {&(init_task), };
char kstack_pool[64][4096];
int task_num = 0;

void foo(){
  while(1) {
    uart_puts("Task id: ");
    uart_int(current -> task_id);
    uart_send('\n');
    delay(10000000000);
  }
}

void idle(){
  while(1){
    delay(100000000);
    schedule();
  }
}

void init_init_task(void (*func)()){
    current->task_id = 0;
    current->counter = 1;
    current->cpu_context.x19 = (unsigned long)func;
    current->cpu_context.pc = (unsigned long)ret_from_fork;
    current->cpu_context.sp = (unsigned long)current + THREAD_SIZE;
}

void privilege_task_create(void(*func)()) {
  struct task *new_task = (struct task_struct *) get_free_page();;
  new_task->task_id = task_num;
  new_task->counter = 3;
  new_task->cpu_context.x19 = (unsigned long)func;
  new_task->cpu_context.pc = (unsigned long)ret_from_fork;
  new_task->cpu_context.sp = (unsigned long)new_task + THREAD_SIZE;
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
  uart_puts("\n");
  */
  switch_to(prev, next);
}

void schedule() {
  current->counter = 3;
  int next;
  next = ((current->task_id)+1) % task_num;
  context_switch(task_list[next]);
}

void counter_check() {
  current->counter--;
  /*
  uart_puts("\nid: ");
  uart_int(current->task_id);
  uart_puts("\ncounter: ");
  uart_int(current->counter);
  uart_puts("\n");
  */
  if (current->counter == 0) {
    enable_irq();
    schedule();
    disable_irq();
  }
}

int N = 2;
void create_tasks() {
  asm("svc 2");
  init_init_task(foo);
  for (int num = 0; num < N; num++)
    privilege_task_create(foo);
}
