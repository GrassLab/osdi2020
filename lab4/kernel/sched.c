#include "sched.h"
#include "libc.h"
#include "mm.h"
#include "entry.h"

static struct task_struct init_task = INIT_TASK;
struct task_struct *current = &(init_task);
struct task_struct *task[NR_TASKS] = {&(init_task),};
unsigned long nr_tasks = 1;

unsigned long unique_id() { return nr_tasks++; }

struct task_struct *privilege_task_create(void (*func)(), unsigned long arg) {
  /* allocate a task struct */
  struct task_struct *p = (struct task_struct *)get_free_page();
  if (!p)
    return p;

  p->pid = unique_id();                      /* create a unique id */
  p->cpu_context.x19 = (unsigned long)func;  /* hold the funtion pointer */
  p->cpu_context.x20 = (unsigned long)arg;   /* hold the argument */
  p->cpu_context.pc = (unsigned long)ret_from_fork;
  p->cpu_context.sp = (unsigned long)p + THREAD_SIZE;

  /* store the task_struct into task[] */
  task[p->pid] = p;




  uart_println("[Task] Create a privilege task @ %x", p);
  uart_println("[Task] The task with pid: %d", p->pid);

  return p;
}

/* internal scheduler */
void _schedule() { /* struct task_struct *p; */ }

void schedule_tail() {

}
/* extern struct task_struct *get_current(); */

void context_switch(struct task_struct *next) {
  if (current == next)
    return;

  struct task_struct *prev = current;

  current = next;

  /* switch to the next */
  cpu_switch_to(prev, next);
}
