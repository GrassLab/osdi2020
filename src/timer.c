#include "peripherals/timer.h"

void arm_core_timer_enable() {
    // enable timer
    register unsigned int enable = 1;
    asm volatile("msr cntp_ctl_el0, %0" : : "r"(enable));
    // set expired time
    register unsigned int expire_period = CORE_TIMER_EXPRIED_PERIOD;
    asm volatile("msr cntp_tval_el0, %0" : : "r"(expire_period));
    // enable timer interrupt
    *CORE0_TIMER_IRQ_CTRL |= 1 << 1;
}

void arm_core_timer_disable() {
    // disable timer
    register unsigned int enable = 0;
    asm volatile("msr cntp_ctl_el0, %0" : : "r"(enable));
    // disable timer interrupt
    *CORE0_TIMER_IRQ_CTRL &= !(1 << 1);
}

void arm_local_timer_enable() {
    unsigned int flag = 0x30000000;        // enable timer and interrupt.
    unsigned int reload = 0xfffffff / 10;  // 0.14 Hz * 10
    *LOCAL_TIMER_CTRL = flag | reload;
}

void arm_local_timer_disable() {
    *LOCAL_TIMER_CTRL &= !(0b11 << 28);  // disable timer and interrupt.
}