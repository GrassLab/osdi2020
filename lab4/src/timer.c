#include "utils.h"
#include "printf.h"
#include "peripherals.h"

#define LOCAL_TIMER_CONTROL_AND_STATUS	0x40000034
#define LOCAL_TIMER_IRQ_CLR				0x40000038

void local_timer_init(){
	// enable timer and interrupt.
	
	// set bit 29: Interrupt enable
	// set bit 28: Timer enable
	unsigned int flag = 0x30000000;
	
	// bit 0:27: Reload value
	unsigned int reload = 100000000;
	
	*((volatile unsigned int *)LOCAL_TIMER_CONTROL_AND_STATUS) =  flag | reload;
}

void local_timer_handler(){
	// clear interrupt and reload.
	// set bit 31: Interrupt flag clear when written as 1
	// set bit 30: Local timer-reloaded when written as 1
	*((volatile unsigned int *)LOCAL_TIMER_IRQ_CLR) =  0xc0000000;
}
