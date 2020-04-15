#include "uart.h"
#include "utils.h"

//extern void enable_core_timer();
void exc_context(unsigned long x0,unsigned long x1,unsigned long x2,unsigned long x3){
	unsigned long elr_el1, esr_el1;
  	unsigned int ec, iss;
	asm volatile ("mrs %0, ELR_EL1\n"
		"mrs %1, ESR_EL1\n":"=r" (elr_el1), "=r" (esr_el1));
	ec = esr_el1 >> 26;
  	iss = esr_el1 & 0xFFFFFF;
	if(iss==0x1){
		print("Exception return address ");
		print_hex(elr_el1);
		print("\n\rException class (EC) ");
		print_hex(ec);
		print("\n\rInstruction specific syndrome (ISS) ");
		print_hex(iss);
		print("\n\r");
	}
	else if(iss==0x0){
		core_timer_enable();
	}
}

void undefined_context(){
	print("undefined_excption\n\r");
}

void count_tick(){
	static unsigned int tick;
	print("Arm timer interrupt, jiffies ");
	print_dec(++tick);
	print("\n\r");
}
