#include "timer.h"

/***** 
 Thus a 19.2 MHz crystal gives 38.4 M pulses/second
 lowest frequency of 38.4/2^28 = 0.14 Hz
 *****/
void local_timer_enable()
{
    unsigned int flag = 0x30000000; // enable timer and interrupt.
    unsigned int reload = 38400000; // 38.4 M, 1 second
    *LOCAL_TIMER_CONTROL_REG = flag | reload;
}

void local_timer_disable()
{
    *LOCAL_TIMER_CONTROL_REG = 0x0;
}

void local_timer_handler()
{
    *LOCAL_TIMER_IRQ_CLR = 0xc0000000; // clear interrupt and reload.
}
