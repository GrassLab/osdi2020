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
    unsigned int irq = get32(IRQ_PENDING_1);
    switch(irq) {
        case (SYSTEM_TIMER_IRQ_1):
            uart_puts("timer\r\n");
            break;
        default:
            uart_puts("Unknown IRQ.\r\n");
    }
}
