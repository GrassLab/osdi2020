#include "uart.h"

int
main (void)
{
  uart_init ();
  uart_puts ("hello\n");
  while (1)
    uart_send (uart_getc ());
  return 0;
}
