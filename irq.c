#include "irq.h"
#include "uart.h"
#include "timer.h"

void irq()
{
    unsigned int arm, arm_local;
    char r;
    arm = *IRQ_BASIC_PENDING;
    arm_local = *CORE0_INTR_SRC;

    if (arm_local & 0x800)
    {
        // can't run in qemu
        // local timer interrupt
        uart_puts("arm local timer: delay 1 s\n");
        local_timer_handler();
    }
    else if (arm_local & 0x2)
    {
        unsigned int delay_time = 1; // second
        // core timer interrupt
        uart_puts("arm core timer: dalay ");
        uart_send_int(delay_time);
        uart_puts(" s\n");
        _core_timer_handler(delay_time);
    }
}