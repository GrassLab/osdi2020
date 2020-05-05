#include "io.h"
#include "task.h"

extern Task *tasks[TASK_SIZE], *current_task, *next_task;

int *switch_to(Task*, Task*);

int tasks_pos(Task *task){
  for(int i = 0; i < TASK_SIZE + 1; i++)
    if(tasks[i] == task) return i;
  return -1;
}

void context_switch(Task *next) {
  Task *prev = current_task;
  current_task = next;
  current_task->status = running;
  printf("next[%d] lr=0x%x sp=0x%x" NEWLINE, next->pid, next->cpu_ctx.lr, next->cpu_ctx.sp);
  printf("0x%x lr = 0x%x" NEWLINE, kernel_process, next->cpu_ctx.lr);
  switch_to(prev, next);
}

void _schedule(){
  preempt_disable();
  printf("current task = %x" NEWLINE, current_task);

  int beg = tasks_pos(current_task);
  int i = (beg + 1) % TASK_SIZE;
  while(i != beg){
    if(tasks[i] && (tasks[i]->status == idle)){
      printf("task[%d], pc = 0x%x, status %d" NEWLINE, tasks[i]->pid, tasks[i]->cpu_ctx.lr, tasks[i]->status);
      break;
    }
    i = (i + 1) % TASK_SIZE;
  }

  printf("select %d" NEWLINE, i);
  if(current_task){
    if(current_task->status == running)
      current_task->status = idle;
    current_task->flag &= ~RESCHED;
  }

  if(current_task == tasks[i]){
    return;
  }

  next_task = tasks[i];
  context_switch(tasks[i]);
}

void schedule_tail() {
  puts("achieve tail");
  preempt_enable();
}


void show_addr(unsigned long a){
  printf("addr %x" NEWLINE, a);
}

void schedule(){
  current_task->counter = 0;
  _schedule();
}

void check_resched(){
  print("check_resched...");
  if(current_task->flag & RESCHED || current_task->status == zombie){
    if(current_task->status == zombie){
      printf("[%d] it's a zombie" NEWLINE, current_task->pid);
    }
    puts("reschedule flag set");
    schedule();
  }
  puts("nothing... do again");
}
