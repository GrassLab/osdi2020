#include "exception.h"
#include "mini_uart.h"
#include "shell.h"

int main(void) {
  gpio_init();
  mini_uart_init();
  exception_init();
  el1_to_el0();
  shell();
}
