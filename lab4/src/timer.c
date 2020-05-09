#undef __ASSEMBLY__
#include "utils.h"
#include "timer.h"
#include "base.h"
#include "config.h"
#include "irq.h"

const unsigned int interval = 20000000;

unsigned int sys_timer_count = 0;
unsigned int curVal = 0;

void timer_init()
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

void disable_all_timer()
{
	core_timer_disable();
	put32(TIMER_C1, 0xffffffff);
	put32(TIMER_CS, TIMER_CS_M1); 
	disable_timer_controller();
}

void disable_timer_controller()
{
	put32(DISABLE_IRQS_1, 1 << 1);
	return;
}


unsigned int read_cntfrq(void)
{
    unsigned int val;
	asm volatile ("mrs %0, cntfrq_el0" : "=r" (val));
    return val;
}

void write_cntp_tval(unsigned int val)
{
	asm volatile ("msr cntp_tval_el0, %0" :: "r" (val));
    return;
}

unsigned int read_cntp_tval(void)
{
    unsigned int val;
	asm volatile ("mrs %0, cntp_tval_el0" : "=r" (val));
    return val;
}



