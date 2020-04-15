#include "irq.h"
#include "utility.h"
#include "timer.h"

void enable_irq()
{
    asm volatile("svc #0x100");
}

void disable_irq()
{
    asm volatile("svc #0x101");
}

void __enable_irq()
{
    asm volatile("msr daifclr, #2");
}

void __disable_irq()
{
    asm volatile("msr daifset, #2");
}

void irq_handler()
{
	unsigned int first_level_irq = getRegister(CORE0_INTERRUPT_SOURCE);
    unsigned int irq_pending_1 = getRegister(IRQ_PENDING_1);
    unsigned int irq_basic_pending = getRegister(IRQ_BASIC_PENDING);
    if (irq_basic_pending & (1<<19)) {
        return;
    }
	if (first_level_irq == 2) {
		core_timer_handler();
	} else {
        local_timer_handler();
    }
}