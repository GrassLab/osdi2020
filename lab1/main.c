#include "uart.h"
#include "shell.h"

int
main (void)
{
  // init stack guard. It should be random, but I'm lazy.
  __stack_chk_guard = (void *) 0xdeadbeef;
  uart_init ();
  uart_puts ("hello\n");
  shell_interactive ();
  return 0;
}
