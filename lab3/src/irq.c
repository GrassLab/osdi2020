#include "utils.h"
#include "entry.h"
#include "irq.h"
#include "../include/peripherals/irq.h"
#include "peripherals/timer.h"

extern unsigned int interval;
extern unsigned int curVal;
unsigned int core_jf   = 1;
unsigned int system_jf = 1;

void handle_irq(void)
{
	unsigned int sec_level_irq = get32(IRQ_PENDING_1);
	unsigned int fir_level_irq = get32(CORE0_INTERRUPT_SOURCE);
	if(sec_level_irq & AUX_IRQ) {
		handle_uart_irq();
		sec_level_irq &= ~AUX_IRQ;
	}
	if (sec_level_irq& SYSTEM_TIMER_IRQ_1) {
		handle_sys_timer_irq();
		sec_level_irq &= ~SYSTEM_TIMER_IRQ_1;
	}
	if (fir_level_irq == 2) {
		handle_core_timer_irq();
	}
	return;
}

void handle_sys_timer_irq( void ) 
{
	curVal = get32(TIMER_CLO);
	curVal += interval;
	put32(TIMER_C1, curVal);
	put32(TIMER_CS, TIMER_CS_M1); // This register is used to record and clear timer channel comparator matches.
	uart_send_string("System timer interrupt, jiffies ");
	uart_send_int(system_jf);
	system_jf += 1;
	if (system_jf == 10) {
		put32(TIMER_C1, 0xffffffff);
		put32(TIMER_CS, TIMER_CS_M1); // This register is used to record and clear timer channel comparator matches.
		disable_timer_controller();
		system_jf = 1;
	}
	//enable_irq();
	//delay(10000000); //test for defferred interrupt IRQ
	return;
}

void handle_core_timer_irq( void )
{
	unsigned int val;
	asm volatile ("mrs %0, cntfrq_el0" : "=r" (val)); // read val
	asm volatile ("msr cntp_tval_el0, %0" :: "r" (val)); // write tval
	uart_send_string("Arm core timer interrupt, jiffies ");
	uart_send_int(core_jf);
	core_jf += 1;
	if (core_jf == 10) {
		core_timer_disable();
		core_jf = 1;
		uart_send_string("# ");
	}
	return;
}
