#include "task.h"
#include "util.h"
#include "sched.h"
#include "io.h"

//#define current get_current();
//extern Task *get_current();

char kstack_pool[TASK_SIZE][STACK_SIZE];
char ustack_pool[TASK_SIZE][STACK_SIZE];

//static Task init_task = INIT_TASK;
Task task_pool[TASK_SIZE] = {
  [0] = {
    {
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      (unsigned long)kernel_process,
      (unsigned long)kstack_pool[0] + STACK_SIZE
    },
    {{[0 ... 30] = 0}, 0, 0, 0}, 0, 0, 0, 1, 0, pending
  },
  [1 ... TASK_SIZE - 1] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {{[0 ... 30] = 0}, 0, 0, 0}, 0, 0, 0, 0, 0, none
  }
};

Task *current_task = 0, *next_task;
Task *tasks[TASK_SIZE] = { &task_pool[0], };

void append_task(Task *task){
  static int ptr = 1;
  tasks[ptr] = task;
  ptr = (ptr + 1) % TASK_SIZE;
}

int unique_pid(){
  static int pid = 1;
  return pid++;
}

void preempt_enable() { current_task->preempt_count--; }
void preempt_disable() { current_task->preempt_count++; }

int talloc(){
  for (int i = 0; i < TASK_SIZE; i++)
    if (task_pool[i].status == none)
      return i;
  return -1;
}

Task *privilege_task_create(void (*func)()) {
  preempt_disable();
  int free_idx = talloc();
  if(free_idx < 0)
    puts("create new task failed");
  else{
    Task *p = task_pool + free_idx;
    p->pid = unique_pid();
    p->flag = 0;
    p->counter = p->priority = current_task->priority;
    p->preempt_count = 1; // disable preemtion until schedule_tail
    p->status = pending;
    p->cpu_ctx.sp = (unsigned long)kstack_pool[free_idx] + STACK_SIZE;
    p->cpu_ctx.fp = (unsigned long)kstack_pool[free_idx] + STACK_SIZE;
    p->cpu_ctx.lr = (unsigned long)func;
    printf("new task [%d] sp = 0x%x pc = 0x%x" NEWLINE,
        task_pool[free_idx].pid, task_pool[free_idx].cpu_ctx.sp, func);
    append_task(task_pool + free_idx);
  }
  preempt_enable();
  return free_idx < 0 ? 0 : task_pool + free_idx;
}

void show_sp(){
  int *sp;
  __asm__ volatile("mov %0, sp":"=r"(sp));
  printf("sp = 0x%x" NEWLINE, sp);
}

#define TASK_(n) void task_ ## n () { \
  puts(#n " begin"); \
  while(1){ \
    puts(#n " running ..."); delay(500000000); \
    puts("sleep done"); \
    show_sp(); \
    if(current_task->flag & RESCHED){ \
      puts("reschedule flag set"); \
      schedule(); \
    } \
  } \
}

TASK_(1); TASK_(2); TASK_(3); TASK_(4);

void kernel_process(){
  puts("here");
  privilege_task_create(task_1);
  privilege_task_create(task_2);
  //privilege_task_create(task_3);
  //privilege_task_create(task_4);
  while(1){
    puts("back to kernel");
    schedule();
  }
}
