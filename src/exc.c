#include "exc.h"
#include "uart.h"

void handler_exc() {
	unsigned long el, elr_el1, esr_el1;

	asm volatile("mrs %0, CurrentEL" : "=r" (el));
	asm volatile("mrs %0, ELR_EL1" : "=r" (elr_el1));
	asm volatile("mrs %0, ESR_EL1" : "=r" (esr_el1));

	uart_puts("Exception level: EL");
	uart_hex(el >> 2);
	uart_puts("\nException return address: 0x");
	uart_hex(elr_el1);
	uart_puts("\nException class (EC): 0x");
	uart_hex(esr_el1 >> 26);
	uart_puts("\nInstruction specific syndrome (ISS): 0x");
	uart_hex(esr_el1 & 0x00FFFFFF);
	uart_puts("\n");
}
