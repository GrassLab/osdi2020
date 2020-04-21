#include "irq.h"
#include "uart.h"
#include "timer.h"

#define IRQ_PENDING_1 (volatile unsigned int *)(MMIO_BASE + 0xb204)

#define AUX_IRQ (1 << 29)
#define UART_IRQ (1 << 57)

void irq_uart0_handler()
{
    uart_send_hex(*UART0_RIS);
}

void irq()
{
    char r;

    // uart
    if ((*IRQ_BASIC_PENDING) & 0x80000)
    {
        irq_uart0_handler();
        *IRQ_BASIC_PENDING &= ~0x80000;
    }

    // Reference: QA7_rev3.4 p.16
    // Local timer interrupt
    else if ((*CORE0_IRQ_SOURCE) & (1 << 11))
    {
        // can't run in qemu
        // local timer interrupt
        uart_puts("arm local timer: delay 1 s\n");
        local_timer_handler();
    }
    // CNTPNSIRQ interrupt
    else if ((*CORE0_IRQ_SOURCE) & (1 << 1))
    {
        unsigned int delay_time = 1; // miilisecond
        // core timer interrupt
        /*
        uart_puts("arm core timer: dalay ");
        uart_send_int(delay_time);
        uart_puts(" s\n");
        */
        _core_timer_handler(delay_time);
    }
}
