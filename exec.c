#include "uart.h"

/**
 * common exception handler
 */
//void exception_handler(unsigned long type, unsigned long esr, unsigned long elr, unsigned long spsr, unsigned long far)
void exception_handler(void)
{
  unsigned long esr,elr;
  asm volatile ("mrs %0, elr_el2\n\t"
		"mrs %1, esr_el2\n\t"
		:"=r"(elr),"=r"(esr)
		);
  
  uart_puts(" Exception return address 0x");
  uart_hex(elr);
  uart_puts("\n");

  uart_puts(" Exception class (EC) 0x");
  uart_hex(esr>>26);
  uart_puts("\n");
  
  uart_puts(" Instruction specific syndrome (ISS) 0x");
  uart_hex(esr&0xffffff);
  uart_puts("\n");

  }
