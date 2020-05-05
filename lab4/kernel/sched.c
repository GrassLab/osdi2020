#include "sched.h"
#include "entry.h"
#include "irq.h"
#include "libc.h"
#include "mm.h"
#include "sched.h"
#include "miniuart.h"
#include "signal.h"

static struct task_struct init_task = INIT_TASK;
struct task_struct *current = &(init_task);
struct task_struct *task[NR_TASKS] = {&(init_task)};
struct task_struct *runqueue;
unsigned long nr_tasks = 0;
unsigned long task_id = 1;

unsigned long unique_id() {  nr_tasks++; return task_id++; }

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

void preempt_enable() {
  current->preempt_count--;
  need_resched();
}



/* internal scheduler */
void _schedule() {
  preempt_disable();

  int next = 0;
  int c = 0;

  struct task_struct *p;

  /* TODO */
  /* check alarm, wake up any interruptible tasks that have got a signal */
  for (int i = 0; i < NR_TASKS; ++i) {
    p = task[i];
    if (p && p->state == TASK_RUNNING && p->signals != 0) {
      /* wake the task */
      next = i;
      goto SWITCH;
    }
  }
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

    /* uart_print("task: ["); */
    /* for (int i = 0; i < NR_TASKS; ++i) { */
    /*   struct task_struct *r = task[i]; */
    /*   uart_print("%d", r->state); */
    /* } */
    /* uart_println(" ]"); */

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

 SWITCH:
  context_switch(task[next]);
  preempt_enable();
}

void schedule_tail() { preempt_enable(); }

void schedule() {
  current->counter = 0;
  _schedule();
}

void pm_daemon() {
  struct task_struct *p;
  while (1) {
    preempt_disable();
    int n = 0;
    for (int i = 0; i < NR_TASKS; ++i) {
      p = task[i];
      /* printer maintainer */
      if (p && *(char *)(p->print_buffer) != 0 && p != current) {
        uart_puts((char*)p->print_buffer);
        *(char*)(p->print_buffer) = 0;
      }

      /* zombie reaper */
      if (p && p->state == TASK_ZOMBIE && p != current) {
        uart_println("[Zombie] reap the zombie task %d", p->pid);
        n = 1;
        /* free the kernel stack */
        free_page((unsigned long)p);

        task[p->pid] = 0;
        nr_tasks--;
      }
    }
    preempt_enable();
    schedule();
  }
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

#define SIGHAND(who, code, arg)  {                                      \
    if (who->signals & code) {                                           \
      (((sig_t *)who->sighand)[SIGKILL])(arg);                           \
    }                                                                   \
  }

void timer_tick() {
  --current->counter;

  /* epoch not used up or the current task can not be premmpt */
  if (current->counter > 0 || current->preempt_count > 0) {
    return;
  }

  /* handle the signal */
  SIGHAND(current, SIGKILL, 0);


  /* used up its epoch or preemptiable */
  /* set the need_reched flag */

  /* if (current->need_reched) { */
  /*   enable_irq(); */
  /*   _schedule(); */
  /*   disable_irq(); */
  /* } else { */
  current->need_reched = 1;
  current->counter = 0;
  /* } */



  /* issue schedule */

  /* current->need_reched = 0; */
  /* enable_irq(); */
  /* _schedule(); */
  /* disable_irq(); */
}
void need_resched() {


  if (current->need_reched) {
    enable_irq();
    current->need_reched = 0;
    _schedule();
    disable_irq();
  }
}



void exit_process() {
  preempt_disable();
  /* for (int i = 0; i < NR_TASKS; i++) { */
  /*   if (task[i] == current) { */
  /*     task[i]->state = TASK_ZOMBIE; */
  /*     break; */
  /*   } */
  /* } */
  /* if (current->stack) { */
  /*   free_page(current->stack); */
  /* } */
  current->state = TASK_ZOMBIE;
  if (current->stack) {
    /* free the user stack */
    free_page(current->stack);
  }

  preempt_enable();
  schedule();
}
