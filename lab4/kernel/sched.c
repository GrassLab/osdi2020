#include "sched.h"
#include "entry.h"
#include "irq.h"
#include "libc.h"
#include "mm.h"
#include "sched.h"

static struct task_struct init_task = INIT_TASK;
struct task_struct *current = &(init_task);
struct task_struct *task[NR_TASKS] = {&(init_task)};
unsigned long nr_tasks = 1;

unsigned long unique_id() { return nr_tasks++; }

struct task_struct *privilege_task_create(void (*func)(), unsigned long num) {
  preempt_disable();

  /* allocate a task struct */
  struct task_struct *p = (struct task_struct *)get_free_page();
  if (!p)
    return p;

  p->cpu_context.x19 = (unsigned long)func; /* hold the funtion pointer */
  p->cpu_context.x20 = (unsigned long)num;  /* hold the argument */
  p->cpu_context.pc = (unsigned long)ret_from_fork;
  p->cpu_context.sp = (unsigned long)p + THREAD_SIZE;

  /* indicate the state of the task */
  p->pid = unique_id(); /* create a unique id */
  p->priority = current->priority;
  p->state = TASK_RUNNING;
  p->counter = p->priority;
  p->preempt_count = 1; // disable preemtion until schedule_tail
  p->need_reched = 0;

  /* store the task_struct into task[] */
  task[p->pid] = p;

  uart_println("[Task] Create a privilege task @ %x", p);
  uart_println("[Task] The task with");
  uart_println("[Task]   pid:      %d", p->pid);
  uart_println("[Task]   priority: %d", p->priority);
  uart_println("[Task]   counter:  %d", p->counter);

  preempt_enable();

  return p;
}

void preempt_disable() { current->preempt_count++; }

void preempt_enable() { current->preempt_count--; }

/* internal scheduler */
void _schedule() {
  preempt_disable();

  int next = 0;
  int c = 0;

  struct task_struct *p;

  /* TODO */
  /* check alarm, wake up any interruptible tasks that have got a signal */
  /* ==== */

  while (1) {
    /* find out the first task that counter != 0 */
    for (int i = 0; i < NR_TASKS; ++i) {
      p = task[i];
      if (p && p->state == TASK_RUNNING && p->counter > c) {
        c = p->counter;
        next = i;
      }
    }
    if (c) {
      break;
    }
    /* re-assign a counter for each task in task[] */
    for (int i = 0; i < NR_TASKS; ++i) {
      p = task[i];
      if (p) {
        p->counter = (p->counter >> 1) + p->priority;
      }
    }
  }

  context_switch(task[next]);
  preempt_enable();
}

void schedule_tail() { preempt_enable(); }

void schedule() {
  current->counter = 0;
  _schedule();
}

/* #define current get_current() */

/* extern struct task_struct *get_current(); */

void context_switch(struct task_struct *next) {
  if (current == next)
    return;

  struct task_struct *prev = current;

  current = next;
#ifdef DEBUG
  uart_println("[Sched] switch to %d", next->pid);
#endif

  /* switch to the next */
  cpu_switch_to(prev, next);
}

void timer_tick() {
  --current->counter;

  /* epoch not used up or the current task can not be premmpt */
  if (current->counter > 0 || current->preempt_count > 0) {
    return;
  }

  /* used up its epoch */
  /* set the need_reched flag */
  current->need_reched = 1;
  current->counter = 0;

  /* issue schedule */
  current->need_reched = 0;
  enable_irq();
  _schedule();
  disable_irq();
}


void exit_process(){
    preempt_disable();
    for (int i = 0; i < NR_TASKS; i++){
        if (task[i] == current) {
            task[i]->state = TASK_ZOMBIE;
            break;
        }
    }
    if (current->stack) {
        free_page(current->stack);
    }
    preempt_enable();
    schedule();
}
