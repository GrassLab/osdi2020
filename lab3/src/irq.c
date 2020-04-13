#include "tools.h"

void irq_router ()
{
  uart_puts ("hello IRQ\n");
  core_timer_handler ();
}