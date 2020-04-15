#include "timer.h"
#define LOCAL_TIMER_CONTROL_REG ((volatile unsigned int*)(0x40000034))
#define LOCAL_TIMER_CONTROL_IRQ_CLR_REG ((volatile unsigned int*)(0x40000038))
#define LOCAL_TIMER_ENABLE (0x30000000)
#define LOCAL_TIMER_RELOAD (25000000)

#define CORE0_TIMER_IRQ_CTRL ((volatile unsigned int*)(0x40000040))
#define EXPIRE_PERIOD 0xfffffff

void local_timer_init() {
//    unsigned int enable = LOCAL_TIMER_ENABLE;
//    unsigned int reload = LOCAL_TIMER_RELOAD;
    *LOCAL_TIMER_CONTROL_REG = ( LOCAL_TIMER_ENABLE | LOCAL_TIMER_RELOAD );
}

void local_timer_handler() {
    *(LOCAL_TIMER_CONTROL_IRQ_CLR_REG) = 0xc0000000;
}

void core_timer_init() {
    register unsigned long r asm("x0");
    r = 1;
    asm volatile( "msr cntp_ctl_el0, x0"); // enable timer
    *CORE0_TIMER_IRQ_CTRL = 2;
}

void core_timer_handler() {
    register unsigned long r asm("x0");
    r = EXPIRE_PERIOD;
    asm volatile("msr cntp_tval_el0, x0");
}