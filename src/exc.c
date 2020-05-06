#include "uart.h"

void exc_function() {
	unsigned long esr_el2, elr_el2;
  	unsigned int ec, iss;
    asm volatile ("mrs %0, ESR_EL1\n": "=r" (esr_el2));
	asm volatile ("mrs %0, ELR_EL1\n": "=r" (elr_el2));
	ec = esr_el2 >> 26;
  	iss = esr_el2 & 0xFFFFFF;
    if (iss == 1) {
	    uart_puts("Exception return address ");
	    uart_hex(elr_el2);
  	    uart_puts("\n\rException class (EC) ");
	    uart_hex(ec);
 	    uart_puts("\n\rInstruction specific syndrome (ISS) ");
	    uart_hex(iss);
	    uart_puts("\n\r");
    } else if (iss == 2) {
        uart_puts("enable time\n");
        enable_irq();
        core_timer_enable();
        //local_timer_enable();
    }
}

void undefined_function(){
	uart_puts("undefined_excption\n\r");
}
