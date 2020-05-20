#include "io.h"
#include "task.h"
#include "util.h"

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
  if(next->mm.pgd) set_pgd(next->mm.pgd);
  switch_to(prev, next);
}

#define availabe(status) ((status) == idle || (status) == running)

void handle_signals(){
  // need re-implement, handle need be done after context switch
  for(int i = 0; i < TASK_SIZE; i++)
    if(tasks[i] && availabe(tasks[i]->status)){
      if(tasks[i]->signals & SIGKILL){
        printf(NEWLINE "============     [%d] becomes zombie     ============" NEWLINE, tasks[i]->pid);
        tasks[i]->status = zombie;
      }
    }
}

void _schedule(){
  preempt_disable();

  handle_signals();

  int max_priority = 0, max_count = 0, next = -1;
  for(int i = 0; i < TASK_SIZE; i++){
    if(tasks[i] && availabe(tasks[i]->status)){
      if(tasks[i]->priority >= max_priority
          && tasks[i]->counter >= max_count){
        max_priority = tasks[i]->priority;
        max_count = tasks[i]->counter;
        next = i;
      }
      tasks[i]->counter++;
    }
  }

  if(current_task){
    current_task->flag &= ~RESCHED;
  }

  if(next < 0 || current_task == tasks[next]){
    current_task->counter = 0;
    preempt_enable(); // tricky place
    return;
  }

  if(current_task->status == running)
    current_task->status = idle;

  next_task = tasks[next];
  next_task->counter = 0;
  context_switch(tasks[next]);
  preempt_enable(); // tricky place
}

void schedule_tail() {
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
  if(current_task->flag & RESCHED || current_task->status == zombie){
    if(current_task->status == zombie){
      printf(NEWLINE "============    [%d] sched meet zombie   ============" NEWLINE,
          current_task->pid);
    }
    schedule();
  }
}

void debug(){
  puts("debug here");
}
