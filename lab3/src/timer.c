#include "utils.h"
#include "printf.h"
#include "peripherals.h"

#define LOCAL_TIMER_CONTROL_REG		0x40000034
#define LOCAL_TIMER_IRQ_CLR			0x40000038

void local_timer_init(){
	// enable timer and interrupt.
	unsigned int flag = 0x30000000;
	unsigned int reload = 25000000;
	*((volatile unsigned int *)LOCAL_TIMER_CONTROL_REG) =  flag | reload;
}

void local_timer_handler(){
	// clear interrupt and reload.
	*((volatile unsigned int *)LOCAL_TIMER_IRQ_CLR) =  0xc0000000;
}
