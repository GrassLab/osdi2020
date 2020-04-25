#include "peripherals/timer.h"

void local_timer_init ()
{
    //29 Interrupt enable (1= enabled)
    //28 Timer enable (1 = enabled)
    unsigned int flag = 0x30000000;	// enable timer and interrupt. MemBaseAddress
    unsigned int reload = 25000000;   // MaxClockRate
    *LOCAL_TIMER_CONTROL_REG = flag | reload;
}


void local_timer_handler ()
{
    *LOCAL_TIMER_IRQ_CLR = 0xc0000000;	// clear interrupt and reload.
}