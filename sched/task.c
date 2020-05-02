#include <list.h>
#include <string.h>
#include "sched.h"

void
privilege_task_create (void (*func) ())
{
  int i;

  for (i = 0; i < POOL_SIZE; ++i)
    if (task_pool[i].task_id == 0)
      break;
  bzero (&task_pool[i].ctx, sizeof (task_pool[i].ctx));
  task_pool[i].task_id = i + 1;
  task_pool[i].ctx.lr = (size_t) func;
  task_pool[i].ctx.sp = (size_t) &task_pool[i].kstack[0x1000];
  list_add_tail (&task_pool[i].list, runqueue);
}

int
do_exec (void (*func) ())
{
  asm volatile ("mov x0, %0\n"
		"msr sp_el0, x0\n"
		"msr spsr_el1, xzr\n"
		"msr elr_el1, %1\n"
		"eret\n"::"r" (&current->stack[0x1000]), "r" (func):"x0");
  return 0;
}

int
sys_exec (void (*func) ())
{
  return do_exec (func);
}
