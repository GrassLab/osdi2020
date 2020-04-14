#include "shell.h"
#include "miniuart.h"
#include "framebuffer.h"
#include "irq.h"

int main(int argc, char *argv[])
{

  /* initialize uart with default clock & baud rate */
  uart_init();

  /* initialize framebuffer */
  lfb_init();

  /* start interactive shell */
  /* while (1); */
  shell();

  return 0;
}
