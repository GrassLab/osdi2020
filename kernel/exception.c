#include <uart.h>
#include <string.h>
#include <timer.h>
#include "shell.h"
#include "irq.h"
#include "panic.h"

void
exception_router ()
{
  unsigned long elr_el1, esr_el1;
  unsigned int ec, iss;

  asm volatile ("mrs %0, elr_el1\n"
		"mrs %1, esr_el1\n":"=r" (elr_el1), "=r" (esr_el1));
  ec = esr_el1 >> 26;
  iss = esr_el1 & 0xffffff;
  if (ec == 0x15)
    {
      if (iss == 0)
	syscall_number_error ();
      else
	show_exception_status (elr_el1, esr_el1);
    }
  else
    {
      unhandled_exception ();
    }
}
