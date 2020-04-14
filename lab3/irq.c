#include "timer.h"
#include "uart.h"

#define LOCAL_TIMER_CONTROL_REG (unsigned int *)0x40000034
#define LOCAL_TIMER_IRQ_CLR (unsigned int *)0x40000038

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
  printf("put s to stop\n");
  enable_irq();
  core_timer_enable();
  while (1) {
    char get = uart_getc();
    if (get == 's') {
      break;
    }
  }
  disable_irq();
}

void irq_router() {
  core_timer_handler();
  printf("time_inturrupt\n");
}
