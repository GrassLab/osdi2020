#include "irq.h"
#include "uart.h"
#include "timer.h"
#include "util.h"
#include "string.h"


// the core can get irq from many sources, test if source is true
#define IRQ_SOURCE(val, bit) ({ \
	int retval = 0; \
	if ( (val) & (1 << (bit)) ) \
		retval = 1; \
	retval; \
}) \

static unsigned int core_timer_print = 0;
static unsigned int local_timer_print = 0;

void handler_lower_el_aarch64_irq() {
	//uart_puts("This is IRQ handler\n");
	unsigned int core0_irq;
	get32(CORE0_IRQ_SOURCE, &core0_irq);
	
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
}

void timer_irq_print(unsigned int enable) {
	core_timer_print = enable;
	local_timer_print = enable;
}
