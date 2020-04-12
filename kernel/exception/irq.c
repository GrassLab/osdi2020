#include "kernel/peripherals/uart.h"

#include "irq.h"
#include "timer.h"

void irq_controller ( )
{
    int irq_source = *CORE0_IRQ_SOURCE;
    int deal = 0;

    if ( irq_source & 0b10 )
    {
        uart_printf("Core Timer Interrupt\n");
        core_timer_reload ();

        deal = 1;
    }
    
    if (irq_source & 0b100000000000 )
    {
        uart_printf("Local Timer Interrupt\n");
        local_timer_reload ();

        deal = 1;
    }

    if ( !deal )
    {
        uart_printf("Some Interrupt Happened. I don't like it\n");
        uart_printf("%x\n", irq_source);
    }
}

