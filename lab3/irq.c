#include "timer.h"
#include "uart.h"

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
