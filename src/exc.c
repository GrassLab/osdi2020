#include "exc.h"

void exc_function(unsigned long x0, unsigned long x1) {
	unsigned long esr_el2, elr_el2;
  	unsigned int ec, iss;
    asm volatile ("mrs %0, ESR_EL1\n": "=r" (esr_el2));
	asm volatile ("mrs %0, ELR_EL1\n": "=r" (elr_el2));
	ec = esr_el2 >> 26;
  	iss = esr_el2 & 0xFFFFFF;
    if (iss == 0) {
      unsigned long syscall;
      unsigned long esr_el1, elr_el1;
      asm volatile("mov %0, x8" : "=r"(syscall));
      asm volatile("mrs %0, esr_el1" : "=r"(esr_el1));
      asm volatile("mrs %0, elr_el1" : "=r"(elr_el1));
      switch (syscall) {
            case 0:
                for(int i = 0; i < x1; i++)
                  uart_send(*((char *)x0 + i));
                break;
            case 1:
                for(int i = 0; i < x1; i++)
                  *((char *)x0 + i) = uart_getc();
                asm volatile("mov x0, %0" : "=r"(x0));
                break;
            case 2:
                do_exec((void (*)())x0);
                break;
            case 3:
                do_fork();
                break;
        }

    } else if (iss == 1) {
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
