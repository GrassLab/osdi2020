#include "exchandler.h"
#include "uart.h"
#include "timer.h"

/****** WE ARE IN EL1 NOW! ******/

void syscall(unsigned int code, long x0, long x1, long x2, long x3, long x4, long x5) {
	switch (code) {
		case 0:
			sys_timer();
			break;
		default:
			sys_undef(code);
			break;
	}
}

void sys_timer() {
	uart_puts("sys_timer\n");
	local_timer_init();
	core_timer_init();
}

void sys_undef(unsigned int code) {
	uart_puts("Undefined system call code: 0x");
	uart_hex(code);
	uart_puts("\n");
}


/* 
 * in Exception class (EC) 15:
 *     Supervisor call (SVC) handler
 *     SVC #(ISS)
 */
void handler_lower_el_aarch64_sync(long x0, long x1, long x2, long x3, long x4, long x5) {
	unsigned long el, elr_el1, esr_el1;

	asm volatile("mrs %0, CurrentEL" : "=r" (el));
	asm volatile("mrs %0, ELR_EL1" : "=r" (elr_el1));
	asm volatile("mrs %0, ESR_EL1" : "=r" (esr_el1));

	// for ESL_EL1: EC [31:26], exception class
	//              ISS [23:0], instruction specific syndrome
	unsigned int ec = esr_el1 >> 26;
	unsigned int iss = esr_el1 & 0xFFFFFF;
	
	if (!ec) { // unknown exception 
		uart_puts("Unknown exception error.");
		// restart main
		asm volatile("adr x0, main");
		asm volatile("msr elr_el1, x0");
		asm volatile("eret");
	}

	if (!iss) { // ISS = 0 (SVC #0)
		unsigned int code;
		asm volatile("mov %0, x8" : "=r" (code));
		syscall(code, x0, x1, x2, x3, x4, x5);
	}
	else {
		// print exception info
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
}

	

