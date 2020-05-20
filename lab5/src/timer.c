#include "utils.h"
#include "printf.h"
#include "sched.h"
#include "peripherals/timer.h"

const unsigned int interval = 200000;
unsigned int curVal = 0;

void timer_init ( void )
{
    curVal = get32(TIMER_CLO);
    curVal += interval;
    put32(TIMER_C1, curVal);
}

void handle_timer_irq( void )
{
    curVal += interval;
    put32(TIMER_C1, curVal);
    put32(TIMER_CS, TIMER_CS_M1);
    timer_tick();
}


void sched_core_timer_handler() {
  unsigned long r;
#ifdef DEBUG
  asm volatile ("mov %0, #0x600000" : "=r"(r));
#else
  asm volatile ("mov %0, #0x200000" : "=r"(r));
#endif
  asm volatile ("msr cntp_tval_el0, %0" : "=r"(r));
  timer_tick();
}
