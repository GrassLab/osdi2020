#include "type.h"
#include "device/uart.h"
#include "task/taskManager.h"

#define CORE0_TIMER_IRQ_CTRL 0x40000040
#define EXPIRE_PERIOD 0xfffffff
#define LOCAL_TIMER_CONTROL ((volatile unsigned int *)0x40000034)
#define LOCAL_TIMER_IRQ_CLR ((volatile unsigned int *)0x40000038)

unsigned int core_timer_count = 0;
unsigned int local_timer_count = 0;

void enableCoreTimer()
{
    asm volatile("mov x0, 1");
    asm volatile("msr cntp_ctl_el0, x0");
    asm volatile("mov x0, 2");
    asm volatile("ldr x1, =0x40000040");
    asm volatile("str x0, [x1]");
    asm volatile("mov x0, 0xffffff");
    asm volatile("msr cntp_tval_el0, x0");

    return;
}

void coreTimerHandler()
{
    // core_timer_count++;
    // uartPuts("Core timer interrupt, jiffies ");
    // uartInt(core_timer_count);
    // uartPuts("\n");

    current->re_schedule = true;

    asm volatile("mov x0, 0x1ffffff");
    asm volatile("msr cntp_tval_el0, x0");

    return;
}

void localTimerInit()
{
    unsigned int flag = 0x30000000; // enable timer and interrupt.
    unsigned int reload = 25000000;
    *LOCAL_TIMER_CONTROL = flag | reload;
}

void localTimerHandler()
{
    local_timer_count++;
    uartPuts("Local timer interrupt, jiffies ");
    uartInt(local_timer_count);
    uartPuts("\n");

    *LOCAL_TIMER_IRQ_CLR = 0xc0000000; // clear interrupt and reload.
}