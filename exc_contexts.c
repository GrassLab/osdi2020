#include "uart.h"
#include "utils.h"

#define ARM_TIMER_LOAD 		((volatile unsigned int*)0x3f00b400)
#define ARM_TIMER_CONTROL 	((volatile unsigned int*)0x3f00b408)
#define ARM_TIMER_RAW_IRQ	((volatile unsigned int*)0x3f00b410)
#define ARM_TIMER_IRQ_CLR 	((volatile unsigned int*)0x3f00b40c)
#define IRQ_ENABLE2 		((volatile unsigned int*)0x3f00b218)

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
	int cel;
	asm("mrs %0,currentEL":"=r"(cel));
	print_dec(cel);
	print("\n\r");
}

void side_timer_enable(){
	*ARM_TIMER_CONTROL=(1<<7)|(1<<5)|(1<<1);
	*ARM_TIMER_LOAD=500000;
	*IRQ_ENABLE2=1;
}

void count_tick(){
	static unsigned int core_tick,side_tick;
	if(*ARM_TIMER_RAW_IRQ){
		*ARM_TIMER_IRQ_CLR=1;
		print("Side timer interrupt, jiffies ");
		print_dec(++side_tick);
		print("\n\r");
	}
	else{
		asm("mov x0, #0xffffff\n"
    		 "msr cntp_tval_el0, x0");
		print("Arm timer interrupt, jiffies ");
		print_dec(++core_tick);
		print("\n\r");
	}
}
