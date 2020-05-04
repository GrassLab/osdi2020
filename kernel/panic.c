#include <string.h>
#include <uart.h>
#include "panic.h"

void
not_implemented ()
{
  printf ("kernel panic - function not implemented!");
  show_exception_status ();
  while (1);
}

void
syscall_number_error ()
{
  printf ("kernel panic - syscall number error!");
  show_exception_status ();
  while (1);
}

void
show_exception_status ()
{
  unsigned long elr_el1, esr_el1;
  unsigned int ec, iss;

  asm volatile ("mrs %0, elr_el1\n"
		"mrs %1, esr_el1\n":"=r" (elr_el1), "=r" (esr_el1));
  ec = esr_el1 >> 26;
  iss = esr_el1 & 0xffffff;
  printf ("Exception return address 0x%p\r\n", (void *) elr_el1);
  printf ("Exception class (EC) 0x%x\r\n", ec);
  printf ("Instruction specific syndrome (ISS) 0x%x\r\n", iss);
}

void
unhandled_exception ()
{
  uart_puts ("kernel panic - unhandled exception\n");
  show_exception_status ();
  while (1);
}
