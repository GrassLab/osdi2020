#include "uart.h"
#include "utils.h"

void exc_context(unsigned long x0,unsigned long x1,unsigned long x2,unsigned long x3){
	unsigned long elr_el2, esr_el2;
  	unsigned int ec, iss;
	asm volatile ("mrs %0, ELR_EL2\n"
		"mrs %1, ESR_EL2\n":"=r" (elr_el2), "=r" (esr_el2));
	ec = esr_el2 >> 26;
  	iss = esr_el2 & 0xFFFFFF;
	print("Exception return address ");
	print_hex(elr_el2);
  	print("\n\rException class (EC) ");
	print_hex(ec);
 	print("\n\rInstruction specific syndrome (ISS) ");
	print_hex(iss);
	print("\n\r");
}

void undefined_context(){
	print("undefined_excption\n\r");
}

void count_tick(){
	static unsigned int tick;
	int t1,t2;
	print("Arm timer interrupt, jiffies ");
	//asm("mrs %0, cntp_cval_el0\n"
	//	"mrs %1, cntpct_el0":"=r"(t1),"=r"(t2));
	print_dec(++tick);
	print("\n\r");
}
