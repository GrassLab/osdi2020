#include "utils.h"
#include "timer.h"
#include "entry.h"
#include "irq.h"
#include "mini_uart.h"
#include "../include/peripherals/irq.h"

void handle_irq(void)
{
	unsigned int fir_level_irq = get32(CORE0_INTERRUPT_SOURCE);
	if (fir_level_irq == 256) {
		handle_sys_timer_irq();
	}
	else if (fir_level_irq == 2) {
		handle_core_timer_irq();
	}
	return;
}

void enable_interrupt_controller()
{
	put32(ENABLE_IRQS_1, SYSTEM_TIMER_IRQ_1);
	return;
}