#include "uart.h"

void exc_function(unsigned long x0, unsigned long x1, unsigned long x2, unsigned long x3){
	unsigned long esr_el2, elr_el2;
  	unsigned int ec, iss;
    asm volatile ("mrs %0, ESR_EL2\n": "=r" (esr_el2));
	asm volatile ("mrs %0, ELR_EL2\n": "=r" (elr_el2));
	ec = esr_el2 >> 26;
  	iss = esr_el2 & 0xFFFFFF;
	uart_puts("Exception return address ");
	uart_hex(elr_el2);
  	uart_puts("\n\rException class (EC) ");
	uart_hex(ec);
 	uart_puts("\n\rInstruction specific syndrome (ISS) ");
	uart_hex(iss);
	uart_puts("\n\r");
}

void undefined_function(){
	uart_puts("undefined_excption\n\r");
}
