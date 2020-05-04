#include "task.h"
#include "fork.h"

extern Task *current_task;
extern char kstack_pool[TASK_SIZE][STACK_SIZE];
extern char ustack_pool[TASK_SIZE][STACK_SIZE];

void do_exec(unsigned long pc){
  current_task->regs.pc = pc;
  current_task->regs.pstate = PSR_MODE_EL0t;
  current_task->regs.sp = (unsigned long)ustack_pool[current_task->pid % TASK_SIZE];
}
