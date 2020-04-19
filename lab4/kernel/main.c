#include "bh.h"
#include "framebuffer.h"
#include "irq.h"
#include "libc.h"
#include "miniuart.h"
#include "sched.h"
#include "shell.h"

void wait_msec(unsigned int n) {
  register unsigned long f, t, r;
  // get the current counter frequency
  asm volatile("mrs %0, cntfrq_el0" : "=r"(f));
  // read the current counter
  asm volatile("mrs %0, cntpct_el0" : "=r"(t));
  // calculate expire value for counter
  t += ((f / 1000) * n) / 1000;
  do {
    asm volatile("mrs %0, cntpct_el0" : "=r"(r));
  } while (r < t);
}

void first_func() {
  while (1) {
    uart_println("(1) first function ...");
    wait_msec(600000);
    context_switch(task[0]);
  }
}

void second_func() {
  while (1) {
    uart_println("(2) second function ...");
    wait_msec(600000);
    context_switch(task[0]);
  }
}

void el1_main() {
  uart_init();

  /* stay at interrupt disabled here */
  privilege_task_create(first_func, 0);
  privilege_task_create(second_func, 0);

  int i = 0;
  while (1) {
    context_switch(task[i++%2+1]);
  }
}

int main(int argc, char *argv[]) {
  bh_mod_mask = 0;

  /* initialize uart with default clock & baud rate */
  uart_init();

  /* initialize framebuffer */
  lfb_init();

  /* start interactive shell */
  /* while (1); */
  shell();

  return 0;
}
