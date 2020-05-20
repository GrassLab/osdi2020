#include "fork.h"
#include "entry.h"
#include "mm.h"
#include "sched.h"
#include "utils.h"

int copy_process(unsigned long clone_flags, unsigned long fn,
                 unsigned long arg) {
  preempt_disable();
  struct task_struct *p;

  p = (struct task_struct *)allocate_kernel_page();
  if (p == 0)
    return -1;

  struct pt_regs *childregs = task_pt_regs(p);
  memzero((unsigned long)childregs, sizeof(struct pt_regs));
  memzero((unsigned long)&p->cpu_context, sizeof(struct cpu_context));
  memzero((unsigned long)&p->mm, sizeof(struct mm_struct));

  if (clone_flags & PF_KTHREAD) {
    p->cpu_context.x19 = fn;
    p->cpu_context.x20 = arg;
  } else {
    struct pt_regs *cur_regs = task_pt_regs(current);
    *cur_regs = *childregs;
    childregs->regs[0] = 0;

    /* copy virtuual memory */
    copy_virt_memory(p);
  }
  p->pid = nr_tasks++;
  p->flags = clone_flags;
  p->priority = current->priority;
  p->state = TASK_RUNNING;
  p->counter = p->priority;
  p->preempt_count = 1; // disable preemtion until schedule_tail

  p->cpu_context.pc = (unsigned long)ret_from_fork;
  p->cpu_context.sp = (unsigned long)childregs;

  task[p->pid] = p;

  preempt_enable();
  return p->pid;
}

int move_to_user_mode(unsigned long start, unsigned long size,
                      unsigned long pc) {
  struct pt_regs *regs = task_pt_regs(current);
  memzero((unsigned long)regs, sizeof(struct pt_regs));
  regs->pstate = PSR_MODE_EL0t;
  regs->pc = pc;
  regs->sp = 2 * PAGE_SIZE;
  unsigned long code_page = allocate_user_page(current, 0);
  if (code_page == 0) {
    return -1;
  }
  memcpy(start, code_page, size);
  set_pgd(current->mm.pgd);
  return 0;
}

int do_exec(unsigned long start, unsigned long size, unsigned long pc) {
  struct pt_regs *regs = task_pt_regs(current);
  memzero((unsigned long)regs, sizeof(struct pt_regs));

  /* setup the child regs for user process */
  regs->pstate = PSR_MODE_EL0t;
  regs->pc = pc;
  regs->sp =  0x0000ffffffffe000;

  unsigned long code_page  = allocate_user_page(current, pc);
  /* unsigned long stack_page = allocate_user_page(current, regs->sp - 0x1000); */
  /* if (code_page == 0 || stack_page == 0) { */
  /*   return -1; */
  /* } */

#include "printf.h"
  println("[do exec] do exec for user process");
  println("|   allocate code page  @ %X from pc: %X", code_page, pc);
  /* println("|_  allocate stack page @ %X from sp: %X", stack_page, regs->sp - 0x1000); */

  memcpy(start, code_page, size);

  // set pgd
  set_pgd(current->mm.pgd);
  return 0;
}

struct pt_regs *task_pt_regs(struct task_struct *tsk) {
  unsigned long p = (unsigned long)tsk + THREAD_SIZE - sizeof(struct pt_regs);
  return (struct pt_regs *)p;
}
