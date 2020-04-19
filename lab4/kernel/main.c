#include "shell.h"
#include "miniuart.h"
#include "framebuffer.h"
#include "irq.h"
#include "bh.h"
#include "sched.h"

void first_func() {

}

void el1_main()
{
  uart_init();

  /* stay at interrupt disabled here */
  int ret = privilege_task_create(first_func);
  ret = privilege_task_create(first_func);

  if (ret != 0) {
    /* error */
  }

}

int main(int argc, char *argv[])
{
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
