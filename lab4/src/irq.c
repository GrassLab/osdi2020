#include "irq.h"
#include "asm.h"
#include "uart.h"

void 
enable_interrupt_controller()
{
	put32(ENABLE_IRQS_1, SYSTEM_TIMER_IRQ_1);
}

void
handle_irq()
{
    uart_puts("hello\r\n");
    unsigned int irq = get32(IRQ_PENDING_1);
    switch(irq) {
        case (SYSTEM_TIMER_IRQ_1):
            uart_puts("timer\r\n");
            break;
        default:
            uart_puts("Unknown IRQ.\r\n");
    }
}

void irq_handler()
{
    unsigned int second_level_irq = get32(IRQ_PENDING_1);
	unsigned int first_level_irq = get32(CORE0_INTERRUPT_SOURCE);

	if (first_level_irq == 2) {
		core_timer_handler();
        delay(1000);
        timer_tick();
	} else {
        local_timer_handler();
    }
}

