#include "uart.h"
#include "framebuffer.h"
#include "shell.h"

int main(int argc, char *argv[])
{
  /* initialize uart with default clock & baud rate */
  uart_init_default();

  /* initialize framebuffer */
  lfb_init();

  /* start interactive shell */
  shell();

  return 0;
}
