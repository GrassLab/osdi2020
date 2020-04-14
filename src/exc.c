#include "exc.h"
#include "uart.h"

void handler_exc() {
	unsigned long elr_el2, esr_el2;

	asm volatile("mrs %0, ELR_EL2" : "=r" (elr_el2));
	asm volatile("mrs %0, ESR_EL2" : "=r" (esr_el2));
	
	uart_puts("Exception return address: 0x");
	uart_hex(elr_el2);
	uart_puts("\nException class (EC): 0x");
	uart_hex(esr_el2 >> 26);
	uart_puts("\nInstruction specific syndrome (ISS): 0x");
	uart_hex(esr_el2 & 0x00FFFFFF);
	uart_puts("\n");
}
