#include "mini_uart.h"
#include "shell.h"

int main(void) {
  gpio_init();
  mini_uart_init();
}
