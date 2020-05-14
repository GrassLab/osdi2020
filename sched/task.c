#include <list.h>
#include <string.h>
#include <irq.h>
#include <tlb.h>
#include "sched.h"

struct task_struct *
privilege_task_create (void (*func) ())
{
  int i;

  for (i = 0; i < POOL_SIZE; ++i)
    if (task_pool[i].task_id == 0)
      break;
  if (i == POOL_SIZE)
    return NULL;
  bzero (&task_pool[i].ctx, sizeof (task_pool[i].ctx));
  task_pool[i].task_id = i + 1;
  task_pool[i].ctx.lr = (size_t) func;
  task_pool[i].kstack = page_alloc_virt (KPGD, 0, STACK_SIZE >> 12);
  task_pool[i].ctx.sp = (size_t) task_pool[i].kstack + STACK_SIZE;
  task_pool[i].signal_map = 0;
  list_add_tail (&task_pool[i].list, runqueue);
  return &task_pool[i];
}

int
do_exec (void (*func) ())
{
  asm volatile ("mov x0, %0\n" "mov sp, %1\n"
		"msr sp_el0, x0\n" "msr spsr_el1, xzr\n" "msr elr_el1, %2\n"
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
		"mov x30, xzr\n" "eret\n"::"r" (&current->stack[STACK_SIZE]),
		"r" (&current->kstack + STACK_SIZE), "r" (func):"x0");
  return 0;
}

int
sys_exec (void (*func) ())
{
  return do_exec (func);
}

size_t
do_get_task_id ()
{
  return current->task_id;
}

size_t
sys_get_task_id ()
{
  return do_get_task_id ();
}

/* syscall only, not in IRQ */
struct trapframe *
get_syscall_trapframe (struct task_struct *task)
{
  struct trapframe *tf;
  tf = (struct trapframe *) (&task->kstack + STACK_SIZE - sizeof (*tf));
  return tf;
}

void
rebase_stack_pointer (size_t fp, size_t old_base, size_t new_base)
{
  size_t *target;
  target = (size_t *) fp;
  while ((size_t) target - old_base < STACK_SIZE)
    {
      *target = new_base + *target - old_base;
      target = (size_t *) *target;
    }
}

int
do_fork ()
{
  struct task_struct *new;
  struct trapframe *tf;
  int pid;
  size_t *sp_el0;
  size_t *x29;

  // create a task not in runqueue
  disable_irq ();
  new = privilege_task_create (&&child);
  list_del (&new->list);
  enable_irq ();
  // check allocated task
  if (!new)
    return -1;
  // save current context and copy to new task
  switch_to (current, current);
  // setup kernel
  memcpy (new->kstack, current->kstack, STACK_SIZE);
  new->ctx = current->ctx;
  new->ctx.fp =
    (size_t) new->kstack + current->ctx.fp - (size_t) current->kstack;
  new->ctx.sp =
    (size_t) new->kstack + current->ctx.sp - (size_t) current->kstack;
  new->ctx.lr = (size_t) &&child;
  // setup user
  memcpy (new->stack, current->stack, STACK_SIZE);
  tf = get_syscall_trapframe (new);
  tf->fp = (size_t) new->stack + tf->fp - (size_t) current->stack;
  tf->sp_el0 = (size_t) new->stack + tf->sp_el0 - (size_t) current->stack;
  rebase_stack_pointer (tf->fp, (size_t) current->stack, (size_t) new->stack);

  // add new task to runqeue tail
  disable_irq ();
  list_add_tail (&new->list, runqueue);
  enable_irq ();

  pid = new->task_id;
  if (pid != 0)
    return pid;
  else
    {
    child:
      asm volatile ("mov x0, xzr");
      return 0;
    }
}

int
sys_fork ()
{
  return do_fork ();
}

void
do_exit (int status)
{
  struct task_struct *cur = current;
  disable_irq ();
  page_free_virt (KPGD, (size_t) cur->kstack, STACK_SIZE >> 12);
  cur->kstack = 0;
  list_del (&cur->list);
  list_add_tail (&cur->list, zombiequeue);
  enable_irq ();
  switch_to (current, get_next_task ());
}

void
sys_exit (int status)
{
  do_exit (status);
}
