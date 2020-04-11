#include "irq.h"
#include "utility.h"

void irq_handler()
{
    // unsigned int second_level_irq = getRegister(IRQ_PENDING_1);
	// unsigned int first_level_irq = getRegister(CORE0_INTERRUPT_SOURCE);
	// if(second_level_irq & AUX_IRQ) {
	// 	handle_uart_irq();
	// 	second_level_irq &= ~AUX_IRQ;
	// }
	// if (second_level_irq & SYSTEM_TIMER_IRQ_1) {
	// 	handle_sys_timer_irq();
	// 	second_level_irq &= ~SYSTEM_TIMER_IRQ_1;
	// }
	// if (first_level_irq == 2) {
	// 	handle_core_timer_irq();
	// }
    // return;
	// asm volatile("eret");
}