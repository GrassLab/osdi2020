#include "utils.h"
#include "peripherals/timer.h"
#include "mini_uart.h"
const unsigned int interval = 1000000;
unsigned int curVal    = 0;
unsigned int core_jf   = 1;
unsigned int system_jf = 1;
void sys_timer_init ( void )
{
	curVal = get32(TIMER_CLO);
	curVal += interval;
	put32(TIMER_C1, curVal);
	return;
}

void handle_sys_timer_irq( void ) 
{
	curVal += interval;
	put32(TIMER_C1, curVal);
	put32(TIMER_CS, TIMER_CS_M1); // This register is used to record and clear timer channel comparator matches.
	uart_send_string("System timer interrupt, jiffies ");
	uart_send_int(system_jf);
	system_jf += 1;
	return;
}

void handle_core_timer_irq( void )
{
	unsigned int val;
	asm volatile ("mrs %0, cntfrq_el0" : "=r" (val)); // read val
	asm volatile ("msr cntp_tval_el0, %0" :: "r" (val)); // write tval
	uart_send_string("Arm core timer interrupt, jiffies ");
	uart_send_int(core_jf);
	core_jf += 1;
	return;
}