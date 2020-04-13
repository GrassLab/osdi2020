#include "tools.h"

void exception_handler (void)
{
    int elr_el2, esr_el2;
    asm volatile ("mrs %0, elr_el2\n"
            "mrs %1, esr_el2\n":"=r" (elr_el2), "=r" (esr_el2));
    int iss = esr_el2 & 0xffffff;
//   printf ("Exception return address 0x%p\r\n", (void *) elr_el2);
//   printf ("Exception class (EC) 0x%x\r\n", esr_el2 >> 26);
//   printf ("Instruction specific syndrome (ISS) 0x%x\r\n", esr_el2 & 0xffffff);
    uart_puts("Exception return address: ");
    uart_hex(elr_el2);
    uart_send('\n');
    uart_puts("Exception class (EC): ");
    uart_hex(esr_el2);
    uart_send('\n');    
    uart_puts("Instruction specific syndrome (ISS): ");
    uart_hex(iss);
    uart_send('\n');
    asm volatile ("eret");
}
void not_implemented ()
{
  uart_puts ("kernel panic - function not implemented!\n");
  while (1);
}