#include "utils.h"
#include "entry.h"
#include "irq.h"
#include "../include/peripherals/irq.h"
#include "peripherals/timer.h"
#include "../include/sched.h"
unsigned int core_jf   = 1;

void handle_el1_irq(void)
{
	unsigned int fir_level_irq = get32(CORE0_INTERRUPT_SOURCE);
	if (fir_level_irq == 2) {
		handle_core_timer_irq();
	}
	return;
}

void handle_irq(void)
{
	unsigned int fir_level_irq = get32(CORE0_INTERRUPT_SOURCE);
	if (fir_level_irq == 2) {
		handle_core_timer_irq();
	}
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
	current->counter -= 1;
	if (current->counter < 0) {
		current->counter = 0;
	}
	return;
}

