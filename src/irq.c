/*
 * src/irq.c
 * for IRQ handlers and settings:
 *     timer interrupt
 *     uart interrupt
 */

#include "irq.h"
#include "uart.h"
#include "timer.h"
#include "util.h"
#include "string.h"


// the core can get irq from many sources, test if source is true
#define IRQ_SOURCE(val, bit) ({ (val) & (1 << (bit)); })

static unsigned int core_timer_print = 0;
static unsigned int local_timer_print = 0;

void irq_init() {
	set32(ENABLE_IRQS_2, (1 << 25));   // enable uart_int
	//set32(ENABLE_IRQS_1, (1 << 29));   // enable aux_int (mini uart)
}

void handler_lower_el_aarch64_irq() {
	//uart_puts("This is IRQ handler\n");
	unsigned int core0_irq;
	unsigned int irq_basic;
	unsigned int irq1;
	unsigned int irq2;

	get32(CORE0_IRQ_SOURCE, &core0_irq);
	get32(IRQ_BASIC_PENDING, &irq_basic);
	get32(IRQ_PENDING_1, &irq1);
	get32(IRQ_PENDING_2, &irq2);

	while(irq_basic || core0_irq) {
		if(IRQ_SOURCE(core0_irq, 1)) {     // core physical timer CNTP
			core_timer_handler();          // clear IRQ and reload timer
			
			if(core_timer_print) {
				static unsigned int core_timer_cnt = 0;
				char tmp[8];
				uart_puts("Core timer interrupt, jiffies ");
				uart_puts(itoa(core_timer_cnt++, tmp));
				uart_puts("\n");
			}

		}
		if (IRQ_SOURCE(core0_irq, 11)) {   // local timer irq
			local_timer_handler();         // clear IRQ and reload timer

			if(local_timer_print) {
				static unsigned int local_timer_cnt = 0;
				char tmp[8];
				uart_puts("Local timer interrupt, jiffies ");
				uart_puts(itoa(local_timer_cnt++, tmp));
				uart_puts("\n");
			}
		}
		if (IRQ_SOURCE(irq2, 25)) {        // uart_int
			uart0_handler();
		}
		// refresh irq pending status
		get32(CORE0_IRQ_SOURCE, &core0_irq);
		get32(IRQ_BASIC_PENDING, &irq_basic);
		get32(IRQ_PENDING_1, &irq1);
		get32(IRQ_PENDING_2, &irq2);
	}
}

void timer_irq_print(unsigned int enable) {
	core_timer_print = enable;
	local_timer_print = enable;
}

