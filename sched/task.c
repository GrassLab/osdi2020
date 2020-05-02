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
  current->ctx.sp = (size_t) &current->kstack[0x1000];
  asm volatile ("mov x0, %0\n"
		"msr sp_el0, x0\n" "msr spsr_el1, xzr\n" "msr elr_el1, %1\n"
		// prevent kernel address leakage
		"mov x0, xzr\n" "mov x1, xzr\n" "mov x2, xzr\n"
		"mov x3, xzr\n" "mov x4, xzr\n" "mov x5, xzr\n"
		"mov x6, xzr\n" "mov x7, xzr\n" "mov x8, xzr\n"
		"mov x9, xzr\n" "mov x10, xzr\n" "mov x11, xzr\n"
		"mov x12, xzr\n" "mov x13, xzr\n" "mov x14, xzr\n"
		"mov x15, xzr\n" "mov x16, xzr\n" "mov x17, xzr\n"
		"mov x18, xzr\n" "mov x19, xzr\n" "mov x20, xzr\n"
		"mov x21, xzr\n" "mov x22, xzr\n" "mov x23, xzr\n"
		"mov x24, xzr\n" "mov x25, xzr\n" "mov x26, xzr\n"
		"mov x27, xzr\n" "mov x28, xzr\n" "mov x29, xzr\n"
		"mov x30, xzr\n" "eret\n"::"r" (&current->stack[0x1000]),
		"r" (func):"x0");
  return 0;
}

int
sys_exec (void (*func) ())
{
  return do_exec (func);
}
