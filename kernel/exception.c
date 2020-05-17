#include <uart.h>
#include <string.h>
#include <timer.h>
#include <sched.h>
#include "shell.h"
#include "irq.h"
#include "panic.h"

void
exception_router ()
{
  unsigned long elr_el1, esr_el1;
  unsigned int ec, iss;
  void *far_el1 = 0;

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
  else if (ec == 0x24)
    {
      asm volatile ("mrs %0, far_el1":"=r" (far_el1));
      printf ("Segmentation fault: %p\r\n", far_el1);
      do_exit (1);
    }
  else
    {
      unhandled_exception ();
    }
}
