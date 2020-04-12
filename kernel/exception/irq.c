#include "irq.h"
#include "kernel/peripherals/uart.h"

void irq_controller ( )
{
    int irq_source = *CORE0_IRQ_SOURCE;
    switch ( irq_source ) 
    {
        case 0b10:
            uart_printf("Core Timer Interrupt\n");
            core_timer_handler ();
            break;
        default:
            uart_printf("Some Interrupt Happened. I don't like it\n");
            break;
    }
} 
