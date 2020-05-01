#include "fork.h"
#include "entry.h"
#include "mem.h"
#include "sched.h"
#include "libc.h"

int copy_process(unsigned long clone_flags, unsigned long fn, unsigned long arg,
                 unsigned long stack) {
  preempt_disable();

  /* allocate a task struct */
  struct task_struct *p = (struct task_struct *)get_free_page();
  if (!p)
    return -1;

  struct pt_regs *childregs = task_pt_regs(p);
  memzero((unsigned long)childregs, sizeof(struct pt_regs));
  memzero((unsigned long)&p->cpu_context, sizeof(struct cpu_context));

  if (clone_flags & PF_KTHREAD) {
    p->cpu_context.x19 = fn;
    p->cpu_context.x20 = arg;
  } else {
    struct pt_regs *cur_regs = task_pt_regs(current);
    *childregs = *cur_regs;
    childregs->regs[0] = 0;
    /**
     *  +---------------+ stack = base
     *  |  task_struct  |
     *  + ------------- +
     *  |               |
     *  |               |
     *  |               |
     *  | ^^^^^^^^^^^^  |
     *  |     stack     |
     *  +---------------+ sp    = stack + PageSize(4KB)
     */
    childregs->sp = stack + PAGE_SIZE;
    p->stack = stack;
  }
  p->pid      = unique_id();
  p->flags    = clone_flags;
  p->priority = current->priority;
  p->state    = TASK_RUNNING;
  p->counter  = p->priority;
  p->preempt_count = 1; // disable preemtion until schedule_tail

  p->cpu_context.pc = (unsigned long)ret_from_fork;
  p->cpu_context.sp = (unsigned long)childregs;
  task[p->pid] = p;

  uart_println("[Task] Copy a process @ %x", p);
  uart_println("[Task] The task with");
  uart_println("[Task]   pid:      %d", p->pid);
  uart_println("[Task]   priority: %d", p->priority);
  uart_println("[Task]   counter:  %d", p->counter);
  uart_println("[Task]   stack:    %x", p->stack);

  preempt_enable();
  return p->pid;
}

int move_to_user_mode(unsigned long pc) {
  struct pt_regs *regs = task_pt_regs(current);
  memzero((unsigned long)regs, sizeof(*regs));
  regs->pc = pc;
  regs->pstate = PSR_MODE_EL0t;
  unsigned long stack = get_user_free_page(); // allocate new user stack

  if (!stack) {
    return -1;
  }
  regs->sp = stack + PAGE_SIZE;
  current->stack = stack;

  char *PSR_STR_ARRAY[16] = {
    [PSR_MODE_EL0t] = "PSR_MODE_EL0t",
    [PSR_MODE_EL1t] = "PSR_MODE_EL1t",
    [PSR_MODE_EL1h] = "PSR_MODE_EL1h",
    [PSR_MODE_EL2t] = "PSR_MODE_EL2t",
    [PSR_MODE_EL2h] = "PSR_MODE_EL2h",
    [PSR_MODE_EL3t] = "PSR_MODE_EL3t",
    [PSR_MODE_EL3h] = "PSR_MODE_EL3h",
};

  uart_println("[Move] Allocate a stack for process %x @ %x", pc, stack);
  uart_println("[Move] pt_regs:");
  uart_println("[Move]   sp    = %x", regs->sp);
  uart_println("[Move]   pc    = %x", regs->pc);
  uart_println("[Move]   pstat = %s", PSR_STR_ARRAY[regs->pstate]);

  return 0;
}

struct pt_regs *task_pt_regs(struct task_struct *tsk) {
  unsigned long p = (unsigned long)tsk + THREAD_SIZE - sizeof(struct pt_regs);
  return (struct pt_regs *)p;
}

void do_exec(void (*func)) {
  struct task_struct *current = get_current();
  uart_println("do exec in pid: %d w/ process @ %x", current->pid, func);

  move_to_user_mode((unsigned long)func);
}
