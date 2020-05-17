#include <timer.h>
#include <uart.h>
#include <stddef.h>
#include <string.h>
#include <sched.h>
#include "irq.h"

void
irq_router ()
{
  unsigned int arm_local;
  arm_local = *CORE0_INTR_SRC;
  if (arm_local & 0x800)
    {
      // local timer interrupt
      uart_puts ("local timer\n");
      local_timer_handler ();
    }
  else if (arm_local & 0x2)
    {
      // core timer interrupt
      current->resched = 1;
      core_timer_handler ();
    }
  else
    {
      uart_puts ("wtf? ghooooost IRQ\n");
    }
}

int
is_local_timer ()
{
  return *LOCAL_TIMER_CONTROL_REG & 0x80000000;
}

int
is_core_timer ()
{
  size_t cntp_ctl_el0;
  asm volatile ("mrs %0, cntp_ctl_el0":"=r" (cntp_ctl_el0));
  return cntp_ctl_el0 & 4;
}

void
init_uart_irq ()
{
  *ENABLE_IRQ2 = 1 << 25;
}
