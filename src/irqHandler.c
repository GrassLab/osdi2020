#include "uart.h"
#include "timer.h"
#include "irqTable.h"

#define IRQ_BASIC   ((volatile unsigned int *)(0x3F00B200))
#define IRQ_PEND1   ((volatile unsigned int *)(0x3F00B204))
#define IRQ_PEND2   ((volatile unsigned int *)(0x3F00B208))
#define IRQ_ENABLE1 ((volatile unsigned int *)(0x3F00B210))
#define GPU_INTERRUPTS_ROUTING ((volatile unsigned int *)(0x4000000C))
#define CORE0_INTERRUPT_SOURCE ((volatile unsigned int *)(0x40000060))
#define CORE_TIMER_IRQ 2
#define LOCAL_TIMER_IRQ 2048

#define LAST(k,n) ((k) & ((1<<(n))-1))
#define MID(k,m,n) LAST((k)>>(m),((n)-(m)))

void excHandler(unsigned long esr, unsigned long elr)
{
    uartPuts("Exception return address ");
    uartHex(elr);
    uartPuts("\nException class (EC) ");
    uartHex(MID(esr, 26, 32));
    uartPuts("\nInstruction specific syndrome (ISS) ");
    uartHex(LAST(esr, 16));
    uartPuts("\n");

    return;
    // while(1);
}

void irqHandler()
{
    unsigned int cis = *CORE0_INTERRUPT_SOURCE;
	if (cis == CORE_TIMER_IRQ)
		coreTimerHandler();
    else if (cis == LOCAL_TIMER_IRQ)
        localTimerHandler();
    else	
        uartPuts("Unknown pending irq\n");
    return;
}