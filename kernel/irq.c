#include <timer.h>
#include <uart.h>
#include <stddef.h>
#include <string.h>
#include "irq.h"

void
irq_router ()
{
  unsigned int arm, arm_local;
  char r;
  arm = *IRQ_BASIC_PENDING;
  arm_local = *CORE0_INTR_SRC;
  //printf ("%x | ", arm);
  //printf ("%x\r\n", arm_local);
  if (arm & 0x80000)
    {
      // uart interrupt
      if (*UART0_RIS & 0x10)	// UARTRXINTR
	{
	  while (*UART0_FR & 0x40)
	    {
	      // receive
	      r = (char) (*UART0_DR);
	      if (!QUEUE_FULL (read_buf))
		{
		  QUEUE_SET (read_buf, r);
		  QUEUE_PUSH (read_buf);
		}
	    }
	  *UART0_ICR = 1 << 4;
	}
      else if (*UART0_RIS & 0x20)	// UARTTXINTR
	{
	  while (!QUEUE_EMPTY (write_buf))
	    {
	      r = QUEUE_GET (write_buf);
	      QUEUE_POP (write_buf);
	      while (*UART0_FR & 0x20)
		asm volatile ("nop");
	      *UART0_DR = r;
	    }
	  *UART0_ICR = 2 << 4;
	}
    }
  else if (arm_local & 0x800)
    {
      // local timer interrupt
      uart_puts ("local timer\n");
      local_timer_handler ();
    }
  else if (arm_local & 0x2)
    {
      // core timer interrupt
      uart_puts ("core timer\n");
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
