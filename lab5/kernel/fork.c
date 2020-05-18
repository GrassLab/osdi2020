#include "fork.h"
#include "entry.h"
#include "libc.h"
#include "mm.h"
#include "sched.h"
#include "signal.h"

char *PSR_STR_ARRAY[16] = {
    [PSR_MODE_EL0t] = "PSR_MODE_EL0t", [PSR_MODE_EL1t] = "PSR_MODE_EL1t",
    [PSR_MODE_EL1h] = "PSR_MODE_EL1h", [PSR_MODE_EL2t] = "PSR_MODE_EL2t",
    [PSR_MODE_EL2h] = "PSR_MODE_EL2h", [PSR_MODE_EL3t] = "PSR_MODE_EL3t",
    [PSR_MODE_EL3h] = "PSR_MODE_EL3h",
};

static struct task_struct *dup_task_struct(struct task_struct *orig) {
  /* allocate new task_struct */
  struct task_struct *p = (struct task_struct *)get_free_page();
  if (!p)
    return 0;
  memzero((unsigned long)p, PAGE_SIZE);

  /* copies all fields from the original one */
  memcpy(p, current, sizeof(struct task_struct));

  return p;
}

int copy_process(unsigned long clone_flags, unsigned long fn, unsigned long arg) {
  preempt_disable();
  struct task_struct *p;

  unsigned long page = allocate_kernel_page();
  p = (struct task_struct *)page;
  struct pt_regs *childregs = task_pt_regs(p);

  if (!p)
    return -1;

  if (clone_flags & PF_KTHREAD) {
    p->cpu_context.x19 = fn;
    p->cpu_context.x20 = arg;
  } else {
    struct pt_regs *cur_regs = task_pt_regs(current);
    *childregs = *cur_regs;
    childregs->regs[0] = 0;
    copy_virt_memory(p);
  }
  p->flags = clone_flags;
  p->priority = current->priority;
  p->state = TASK_RUNNING;
  p->counter = p->priority;
  p->preempt_count = 1; // disable preemtion until schedule_tail
  p->print_buffer = allocate_kernel_page();
  memzero(p->print_buffer, PAGE_SIZE);

  p->cpu_context.pc = (unsigned long)ret_from_fork;
  p->cpu_context.sp = (unsigned long)childregs;
  p->pid = unique_id();
  task[p->pid] = p;

  preempt_enable();
  return p->pid;
}

/* int copy_process(unsigned long clone_flags, unsigned long fn, unsigned long
 * arg, */
/*                  unsigned long stack) { */
/*   preempt_disable(); */

/*   struct task_struct *p; */

/*   /\* allocate a task struct *\/ */
/*   if (clone_flags & PF_FORK) { */
/*     p = dup_task_struct(current); */
/* #ifdef DEBUG */
/*     uart_println("[Fork] Allocate a new task struct @ %x & copies field from
 * %x w/ pid: %d", p, current, current->pid); */
/* #endif */
/*   } else { */
/*     p = (struct task_struct *)get_free_page(); */
/*     memzero((unsigned long)p, PAGE_SIZE); */
/*   } */

/*   if (!p) */
/*     return -1; */

/*   struct pt_regs *childregs = task_pt_regs(p); */
/*   memzero((unsigned long)childregs, sizeof(struct pt_regs)); */
/*   memzero((unsigned long)&p->cpu_context, sizeof(struct cpu_context)); */

/*   if (clone_flags & PF_KTHREAD) { */

/*     p->cpu_context.x19 = fn; */
/*     p->cpu_context.x20 = arg; */
/*     p->cpu_context.sp = (unsigned long)childregs; */

/*     /\** */
/*      *  +---------------+ stack = base */
/*      *  |  task_struct  | */
/*      *  + ------------- + */
/*      *  |               | */
/*      *  |               | */
/*      *  |               | */
/*      *  | ^^^^^^^^^^^^  | */
/*      *  |     stack     | */
/*      *  +---------------+ sp    = childregs */
/*      *  |   childregs   | */
/*      *  +---------------+ */
/*      *\/ */

/*   } else if (clone_flags & PF_FORK) { */
/*     struct pt_regs *cur_regs = task_pt_regs(current); */

/*     /\* /\\* copy kernel stack *\\/ *\/ */
/*     /\* memcpy(p, current, PAGE_SIZE); *\/ */

/*     /\* copty stack to the new stack *\/ */
/*     memcpy((void*)stack, (const void*)current->stack, PAGE_SIZE); */

/*     *childregs = *cur_regs;     /\* new pt_regs <- old pt_regs *\/ */
/*     childregs->regs[0] = 0;      /\* aissgn the return value *\/ */

/*     /\* adust the offset of kernel and user stack *\/ */
/*     /\* unsigned long kstack_off = current->cpu_context.sp - (unsigned
 * long)current; *\/ */
/*     unsigned long ustack_off = cur_regs->sp - (unsigned long)current->stack;
 */

/*     /\* kernel stack adjust *\/ */
/*     /\* p->cpu_context.sp = (unsigned long)p + kstack_off; *\/ */
/*     // p->cpu_context.sp = (unsigned long)childregs + kstack_off */
/*     /\* p->cpu_context.sp = (unsigned long)p  + kstack_off; *\/ */
/*     p->cpu_context.sp = (unsigned long)childregs; */
/*     /\* /\\* user stack adjust *\\/ *\/ */
/*     childregs->sp     = stack + ustack_off; */
/*     /\* childregs->sp = stack + PAGE_SIZE; *\/ */

/*     /\* assign the stack to newer one *\/ */
/*     p->stack = stack; */

/*     /\* old *\/ */
/*     /\* p->cpu_context.sp = (unsigned long)childregs; *\/ */
/*     /\* childregs->sp = stack + PAGE_SIZE; *\/ */
/*     /\* p->stack = stack; *\/ */
/*     /\* p->cpu_context.sp = (unsigned long)childregs; *\/ */
/* #ifdef DEBUG */
/*     uart_println("[Fork] Allocate a user stack for process %x @ %x", p,
 * stack); */
/*     uart_println("[Fork] pt_regs:"); */
/*     uart_println("[Fork]   sp    = %x", childregs->sp); */
/*     uart_println("[Fork]   pc    = %x", childregs->pc); */
/*     uart_println("[Fork]   pstat = %s", PSR_STR_ARRAY[childregs->pstate]); */
/* #endif */
/*   } else { */
/*     /\* copty stack to the new stack *\/ */
/*     struct pt_regs *cur_regs = task_pt_regs(current); */
/*     *childregs = *cur_regs; */
/*     childregs->regs[0] = 0; */

/*     /\** */
/*      *                +---stack-----------------------------------------+ */
/*      *                |                                                 | */
/*      *                |            copied from fn                       | */
/*      *  +-------------|-+        +---------------+                      | */
/*      *  |  task_struct  |        |  task_struct  |---cup_context.sp
 * ----|---------------+ */
/*      *  + ------------- +        + ------------- +                      |
 * | */
/*      *  |               |        |               |                      |
 * | */
/*      *  |               |        |               |   +-- reg.0 = 0      v
 * | */
/*      *  |               |        |               |   |__ sp -------+
 * +-------+ stack | */
/*      *  + ------------- + assign + ------------- +   |             |    |
 * 4KB  |       | */
/*      *  |   cur_regs    |  =>    |   childregs   | --+             +-->
 * +-------+       | */
/*      *  +---------------+        +---------------+ <------+ | */
/*      *                                                    | | */
/*      * +-----------------------------+ */
/*      *\/ */

/*     childregs->sp = stack + PAGE_SIZE; */
/*     p->stack = stack; */
/*     p->cpu_context.sp = (unsigned long)childregs; */
/*   } */
/*   p->pid      = unique_id(); */
/*   p->flags    = clone_flags; */
/*   p->priority = current->priority; */
/*   p->state    = TASK_RUNNING; */
/*   p->counter  = p->priority; */
/*   p->preempt_count = 1; // disable preemtion until schedule_tail */

/*   p->signals = 0; */

/*   p->sighand = get_free_page(); */
/*   memzero(p->sighand, PAGE_SIZE); */

/*   /\* TODO it's not a good way to record signalhandler *\/ */
/*   ((sig_t*)p->sighand)[SIGKILL] = (sig_t)exit_process; */

/*   p->print_buffer = get_free_page(); */
/*   memzero(p->print_buffer, PAGE_SIZE); */

/*   p->cpu_context.pc = (unsigned long)ret_from_fork; */

/*   task[p->pid] = p; */

/* #ifdef DEBUG */
/*   uart_println("[Task] Copy a process @ %x", p); */
/*   uart_println("[Task] The task with"); */
/*   uart_println("[Task]   pid:      %d", p->pid); */
/*   uart_println("[Task]   flags:    %d", p->flags); */
/*   uart_println("[Task]   priority: %d", p->priority); */
/*   uart_println("[Task]   counter:  %d", p->counter); */
/*   uart_println("[Task]   stack:    %x", p->stack); */
/* #endif */

/*   preempt_enable(); */
/*   return p->pid; */
/* } */

int move_to_user_mode(unsigned long start, unsigned long size,
                      unsigned long pc) {
  struct pt_regs *regs = task_pt_regs(current);
  memzero((unsigned long)regs, sizeof(*regs));
  regs->pstate = PSR_MODE_EL0t;
  regs->pc = pc;
  regs->sp = 2 * PAGE_SIZE;
  unsigned long code_page = allocate_user_page(current, 0);
  if (code_page == 0) {
    return -1;
  }
  memcpy(code_page, start, size);
  set_pgd(current->mm.pgd);

#ifdef DEBUG
  //uart_println("[Move] Allocate a user stack for process %x @ %x", pc, stack);
  uart_println("[Move] pt_regs:");
  uart_println("[Move]   sp    = %x", regs->sp);
  uart_println("[Move]   pc    = %x", regs->pc);
  uart_println("[Move]   pstat = %s", PSR_STR_ARRAY[regs->pstate]);
#endif
  return 0;
}

/* int move_to_user_mode(unsigned long pc) { */
/*   struct pt_regs *regs = task_pt_regs(current); */
/*   memzero((unsigned long)regs, sizeof(*regs)); */
/*   regs->pc = pc; */
/*   regs->pstate = PSR_MODE_EL0t; */
/*   unsigned long stack = get_user_free_page(); // allocate new user stack */

/*   if (!stack) { */
/*     return -1; */
/*   } */
/*   regs->sp = stack + PAGE_SIZE; */
/*   current->stack = stack; */

/* #ifdef DEBUG */
/*   uart_println("[Move] Allocate a user stack for process %x @ %x", pc,
 * stack); */
/*   uart_println("[Move] pt_regs:"); */
/*   uart_println("[Move]   sp    = %x", regs->sp); */
/*   uart_println("[Move]   pc    = %x", regs->pc); */
/*   uart_println("[Move]   pstat = %s", PSR_STR_ARRAY[regs->pstate]); */
/* #endif */

/*   return 0; */
/* } */

struct pt_regs *task_pt_regs(struct task_struct *tsk) {
  unsigned long p = (unsigned long)tsk + THREAD_SIZE - sizeof(struct pt_regs);
  return (struct pt_regs *)p;
}

int do_exec(void(*func)) {
  /* struct task_struct *current = get_current(); */

#ifdef DEBUG
  uart_println("do exec in pid: %d w/ process @ %x", current->pid, func);
#endif

  /* the process is not only the priviledge task */
  if (current->flags != PF_KTHREAD) {
#ifdef DEBUG
    uart_println("[EXEC] replace the current user stack to newr one");
#endif
    struct pt_regs *regs = task_pt_regs(current);
    memzero((unsigned long)regs, sizeof(*regs));
    regs->pc = (unsigned long)func;
    regs->sp = current->stack + PAGE_SIZE;
    regs->pstate = PSR_MODE_EL0t;

    return 0;
  }


  int err = 0; /* move_to_user_mode((unsigned long)func); */
  if (err < 0) {
    uart_println("Error while moving process to user mode");
  }
  return err;
}

int do_fork() {

  return copy_process(0, 0, 0);
#ifdef DEBUG
  uart_println("do fork in process @ %x w/ pid: %d", current, current->pid);
#endif
  unsigned long stack = get_user_free_page();
  memzero(stack, PAGE_SIZE);
#ifdef DEBUG
  /* uart_println("[Fork] Allocate a new stack @ %x", stack); */
#endif
#ifdef DEBUG
  return copy_process(PF_FORK, 0, 0);
#else
  return copy_process(PF_FORK, 0, 0);
#endif
}
