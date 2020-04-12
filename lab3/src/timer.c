#include "utils.h"
#include "timer.h"
#include "base.h"
#include "config.h"

const unsigned int interval = 20000000;

unsigned int sys_timer_count = 0;
unsigned int curVal = 0;

void timer_init ()
{
	curVal = get32(TIMER_CLO);
	curVal += interval;
	put32(TIMER_C1, curVal);
}

void handle_sys_timer_irq() 
{
	curVal += interval;
	put32(TIMER_C1, curVal);
	put32(TIMER_CS, TIMER_CS_M1); 
	_print("System timer interrupt, jiffies ");
	uart_send_int(sys_timer_count);
	sys_timer_count += 1;
	return;
}

void local_timer_init()
{
	unsigned int flag = 0x30000000; // enable timer and interrupt.
	unsigned int reload = 25000000;
	*LOCAL_TIMER_CONTROL_REG = (flag | reload);
}


