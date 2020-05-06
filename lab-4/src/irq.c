#include "irq.h"
#include "utility.h"
#include "time.h"
#include "uart.h"
#include "task.h"
#include "syscall.h"

void enable_irq()
{
    asm volatile("mov x8, %0" : :"r"(SYSCALL_ENABLE_IRQ));
    asm volatile("svc #0");
}

void disable_irq()
{
    asm volatile("mov x8, %0" : :"r"(SYSCALL_DISABLE_IRQ));
    asm volatile("svc #0");
}

void __enable_irq()
{
    asm volatile("msr daifclr, 0xf");
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
        if (getRegister(UART0_MIS) & (1<<5)) { // send
            setRegister(UART0_ICR, 1 << 5);
        } else if (getRegister(UART0_MIS) & (1<<4)) { // get
            uart_save((char)(getRegister(UART0_DR)));
            setRegister(UART0_ICR, 1 << 4);
        }
        return;
    }
	if (first_level_irq == 2) {
		core_timer_handler();
	} else {
        local_timer_handler();
    }
    irq_reschedule();
}

void irq_reschedule()
{
    Task* task = get_current();
    if (task->rescheduleFlag == 1) {
        // uart_puts("[IRQ reschedule]");
        // uart_print_int(task->id);
        // uart_puts("\n");
        task->rescheduleFlag = 0;
        schedule();
        // uart_puts("[IRQ reschedule]: Finish.\n");
    }
}