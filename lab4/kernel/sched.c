#include "sched.h"
#include "mm.h"
#include "libc.h"

unsigned long unique_id() {
  static unsigned long n = 0;
  return n++;
}

int privilege_task_create(void(*func)()) {
  /* allocate a task struct */
  struct task_struct *p = (struct task_struct *)get_free_page();
  if (!p) return 1;

  p->pid = unique_id();

  uart_println("[Task] Create a privilege task @ %x", p);
  uart_println("[Task] The task with pid: %d", p->pid);


  return 0;
}
