#include "utils.h"
#include "timer.h"
#include "entry.h"
#include "irq.h"
#include "mini_uart.h"
#include "../include/peripherals/irq.h"

void handle_irq(void)
{
	unsigned int irq = get32(IRQ_PENDING_1);
	switch (irq) {
		case (SYSTEM_TIMER_IRQ_1):
			handle_timer_irq();
			break;
		default:
            uart_send_hex(irq);
	}
}

void enable_interrupt_controller()
{
	put32(ENABLE_IRQS_1, SYSTEM_TIMER_IRQ_1);
}