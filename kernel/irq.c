#include <timer.h>
#include <uart.h>

void
irq_router ()
{
  uart_puts ("hello IRQ\n");
  core_timer_handler ();
}
