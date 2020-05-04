#include "utils.h"
#include "peripherals/timer.h"
#include "mini_uart.h"
#include "../include/peripherals/irq.h"

const unsigned int interval = SYSTEM_TIMER_INTERVAL;
unsigned int curVal    = 0;

void enable_timer_controller()
{
	put32(ENABLE_IRQS_1, SYSTEM_TIMER_IRQ_1);
	return;
}

void disable_timer_controller()
{
	put32(DISABLE_IRQS_1, 1 << 1);
	return;
}

void sys_timer_init ( void )
{
	curVal = get32(TIMER_CLO);
	curVal += interval;
	put32(TIMER_C1, curVal);
	return;
}

void enable_core_timer(void)
{
    unsigned int cntfrq;
    unsigned int val;
    cntfrq = read_cntfrq();
    write_cntp_tval(cntfrq);    // clear cnthp interrupt and set next 1 sec timer.
    val = read_cntp_tval();
    core_timer_enable();
    enable_irq();
}