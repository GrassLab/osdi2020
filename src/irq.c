#include "irq.h"
#include "uart.h"


void handler_lower_el_aarch64_irq() {
	uart_puts("This is IRQ handler\n");
}
