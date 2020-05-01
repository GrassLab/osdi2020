#include "type.h"
#include "device/uart.h"
#include "interrupt/timer.h"
#include "interrupt/irqTable.h"
#include "interrupt/irq.h"
#include "task/taskManager.h"

#define IRQ_BASIC ((volatile uint32_t *)(0x3F00B200))
#define IRQ_PEND1 ((volatile uint32_t *)(0x3F00B204))
#define IRQ_PEND2 ((volatile uint32_t *)(0x3F00B208))
#define IRQ_ENABLE1 ((volatile uint32_t *)(0x3F00B210))
#define GPU_INTERRUPTS_ROUTING ((volatile uint32_t *)(0x4000000C))
#define CORE0_INTERRUPT_SOURCE ((volatile uint32_t *)(0x40000060))
#define CORE_TIMER_IRQ 2
#define LOCAL_TIMER_IRQ 2048

#define SVC 21

#define LAST(k, n) ((k) & ((1 << (n)) - 1))
#define MID(k, m, n) LAST((k) >> (m), ((n) - (m)))

void _systemCall()
{
    uint64_t sys_call_no;
    asm volatile("mov %0, x8"
                 : "=r"(sys_call_no));

    switch (sys_call_no)
    {
    case 0:
        _sysFork();
        break;
    case 1:
        _sysexec();
        break;
    default:
        break;
    }
}

void _printEXCInfo(uint64_t elr, uint64_t ec, uint64_t iss)
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

void excHandler(uint64_t esr, uint64_t elr)
{
    uint64_t iss = LAST(esr, 16);
    uint64_t ec = MID(esr, 26, 32);

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
    uint32_t cis = *CORE0_INTERRUPT_SOURCE;
    if (cis == CORE_TIMER_IRQ)
        coreTimerHandler();
    else if (cis == LOCAL_TIMER_IRQ)
        localTimerHandler();
    else
        uartPuts("Unknown pending irq\n");
    return;
}