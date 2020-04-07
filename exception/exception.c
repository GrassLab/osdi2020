#include <uart.h>
#include <stddef.h>
#include <string.h>

void
exception_handler (void)
{
  size_t elr_el2, esr_el2;
  asm volatile ("mrs %0, elr_el2\n"
		"mrs %1, esr_el2\n":"=r" (elr_el2), "=r" (esr_el2));
  printf ("Exception return address 0x%p\r\n", (void *) elr_el2);
  printf ("Exception class (EC) 0x%x\r\n", esr_el2 >> 26);
  printf ("Instruction specific syndrome (ISS) 0x%x\r\n", esr_el2 & 0xffffff);
  asm volatile ("eret");
}
