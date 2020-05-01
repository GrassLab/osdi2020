#include <uart.h>
#include <string.h>
#include <timer.h>
#include "shell.h"
#include "irq.h"

void system_call (size_t x0, size_t x1, size_t x2, size_t x3);

void
show_exception_status (size_t elr_el1, size_t esr_el1)
{
  unsigned int ec, iss;

  ec = esr_el1 >> 26;
  iss = esr_el1 & 0xffffff;
  printf ("Exception return address 0x%p\r\n", (void *) elr_el1);
  printf ("Exception class (EC) 0x%x\r\n", ec);
  printf ("Instruction specific syndrome (ISS) 0x%x\r\n", iss);
}

/* follow aarch64 calling convention in system call */
/* syscall_x0 (x1, x2, x3) */
void
exception_router (size_t x0, size_t x1, size_t x2, size_t x3)
{
  unsigned long elr_el1, esr_el1;
  unsigned int ec, iss;

  asm volatile ("mrs %0, elr_el1\n"
		"mrs %1, esr_el1\n":"=r" (elr_el1), "=r" (esr_el1));
  ec = esr_el1 >> 26;
  iss = esr_el1 & 0xffffff;
  if (ec == 0x15)
    {
      enable_irq ();
      if (iss == 0)
	system_call (x0, x1, x2, x3);
      else
	show_exception_status (elr_el1, esr_el1);
      disable_irq ();
    }
  else
    {
      uart_puts ("kernel panic - unhandled exception\n");
      show_exception_status (elr_el1, esr_el1);
      while (1);
    }
}

void
system_call (size_t x0, size_t x1, size_t x2, size_t x3)
{
  switch (x0)
    {
    case 0:
      sys_core_timer_enable ();
      break;
    case 1:
      sys_get_time ((size_t *) x1, (size_t *) x2);
      break;
    }
}
