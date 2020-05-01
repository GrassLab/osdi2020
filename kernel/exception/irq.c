#include "kernel/peripherals/uart.h"
#include "kernel/task/schedule.h"

#include "irq.h"
#include "timer.h"

void irq_controller_el1 ( )
{
    int irq_source = *CORE0_IRQ_SOURCE;
    int deal = 0;

    if ( irq_source & 0b10 )
    {
        sys_printk("Core Timer Interrupt\n");
        sys_core_timer_reload ();

        /* call for potential rescheduling */
        sys_do_schedule ( );

        deal = 1;
    }
    
    if (irq_source & 0b100000000000 )
    {
        sys_printk("Local Timer Interrupt\n");
        sys_local_timer_reload ();

        deal = 1;
    }

    if ( !deal )
    {
        sys_printk("Some Interrupt Happened. I don't like it\n");
        sys_printk("%x\n", irq_source);
    }
}