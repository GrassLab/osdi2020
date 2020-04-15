#include "timer.h"
#include "uart.h"

#define LOCAL_TIMER_CONTROL_REG (unsigned int *)0x40000034
#define LOCAL_TIMER_IRQ_CLR (unsigned int *)0x40000038
unsigned int core_jf = 0;

void local_timer_init() {
  unsigned int flag = 0x30000000; // enable timer and interrupt.
  unsigned int reload = 25000000;
  *LOCAL_TIMER_CONTROL_REG = (flag | reload);
}

void local_timer_handler() {
  *LOCAL_TIMER_IRQ_CLR = 0xc0000000; // clear interrupt and reload.
  asm volatile("msr cntp_tval_el0, x0");
}

void irq() {
  core_jf = 0;
  asm volatile("mov x0, #0\n"
               "svc #0\n");
}

void core_timer_handler() {
  unsigned int val;
  printf("arm core timer , %d \n", core_jf);
  asm volatile("mrs %0, cntfrq_el0" : "=r"(val));   // read val
  asm volatile("msr cntp_tval_el0, %0" ::"r"(val)); // write tval
  core_jf += 1;
  if (core_jf == 10) {
    core_timer_disable();
    core_jf = 1;
  }
  return;
}

void irq_router_do() { core_timer_handler(); }
