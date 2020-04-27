#include "kernel/peripherals/uart.h"
#include "kernel/task/schedule.h"
#include "kernel/task/task.h"

#include "irq.h"
#include "timer.h"

void irq_controller_el1 ( )
{
    int irq_source = *CORE0_IRQ_SOURCE;
    int deal = 0;

    if ( irq_source & 0b10 )
    {
        uart_printf("Core Timer Interrupt\n");
        core_timer_reload ();

        /* update current task counter */
        task_t * current_task = get_current_task ();
        (current_task -> counter) --;
        
        /* check if need to do reschedule */
        if ( current_task -> counter == 0 )
            RESCHED_FLAG = 1;

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

void irq_controller_el2 ( )
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

