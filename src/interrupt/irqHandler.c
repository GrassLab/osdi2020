#include "device/uart.h"
#include "interrupt/timer.h"
#include "interrupt/irqTable.h"
#include "interrupt/irq.h"
#include "task/taskManager.h"

#define IRQ_BASIC   ((volatile unsigned int *)(0x3F00B200))
#define IRQ_PEND1   ((volatile unsigned int *)(0x3F00B204))
#define IRQ_PEND2   ((volatile unsigned int *)(0x3F00B208))
#define IRQ_ENABLE1 ((volatile unsigned int *)(0x3F00B210))
#define GPU_INTERRUPTS_ROUTING ((volatile unsigned int *)(0x4000000C))
#define CORE0_INTERRUPT_SOURCE ((volatile unsigned int *)(0x40000060))
#define CORE_TIMER_IRQ 2
#define LOCAL_TIMER_IRQ 2048

#define SVC 21

#define LAST(k,n) ((k) & ((1<<(n))-1))
#define MID(k,m,n) LAST((k)>>(m),((n)-(m)))

void _systemCall()
{
    unsigned long sys_call_no;
    asm volatile("mov %0, x8"
                 : "=r"(sys_call_no));
    
    switch (sys_call_no)
    {
    case 0:
        _sysFork();
        break;
    
    default:
        break;
    }
}

void _printEXCInfo(unsigned long elr, unsigned long ec, unsigned long iss)
{
    uartPuts("Exception return address ");
    uartHex(elr);
    uartPuts("\nException class (EC) ");
    uartHex(ec);
    uartPuts("\nInstruction specific syndrome (ISS) ");
    uartHex(iss);
    uartPuts("\n");
}

void _enableTimer()
{
    uartPuts("Enable timer \n");
    enableCoreTimer();
    // localTimerInit();
    enableIrq();
}

void excHandler(unsigned long esr, unsigned long elr)
{
    unsigned long iss = LAST(esr, 16);
    unsigned long ec = MID(esr, 26, 32);

    if (ec == SVC) 
    {
        switch (iss)
        {
        case 0:
            _systemCall();
            break;
        case 1:
            _printEXCInfo(elr, iss, ec);
            break;
        case 2:
            _enableTimer();
            break;
        default:
            break;
        }
    }
    

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