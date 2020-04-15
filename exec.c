#include "uart.h"
#include "time.h"

void irq_enable(void);


void exception_handler(void)
{
  unsigned long esr,elr;
  unsigned int  ec,iss;
  asm volatile ("mrs %0, elr_el2\n\t"
		"mrs %1, esr_el2\n\t"
		:"=r"(elr),"=r"(esr)
		);
  ec = esr>>26;
  iss= esr&0xffffff;
  if(iss){	  
      uart_puts(" Exception return address 0x");
      uart_hex(elr);
      uart_puts("\n");

      uart_puts(" Exception class (EC) 0x");
      uart_hex(ec);
      uart_puts("\n");
  
      uart_puts(" Instruction specific syndrome (ISS) 0x");
      uart_hex(iss);
      uart_puts("\n");
  }
  else{
      irq_enable();
      core_timer_enable();
  }
}
void irq_enable(){
	asm volatile ("mrs     x0,hcr_el2\n\t"
		      "orr     x0, x0, #(1 << 4)\n\t"
    		      "msr     hcr_el2, x0\n\t"
		      "msr     daif, xzr\n\t"
		     );
}
/*void irq_disable(){
	asm volatile ("mrs     x0,hcr_el2\n\t"
		      "orr     x0, x0, #(1 << 4)\n\t"
    		      "msr     hcr_el2, x0\n\t"
		      "msr     daif, xzr\n\t"
		     );
}*/
void irq_handler(void)
{
  	uart_puts(" core timer interrupt\n");
	core_timer_handler();
}
