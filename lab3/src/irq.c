#include "utils.h"
#include "timer.h"
#include "entry.h"
#include "irq.h"
#include "../include/peripherals/irq.h"

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