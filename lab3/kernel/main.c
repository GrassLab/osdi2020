#include "shell.h"
#include "miniuart.h"
#include "framebuffer.h"
#include "irq.h"
#include "bh.h"

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
