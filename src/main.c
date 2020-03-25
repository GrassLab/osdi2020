#include "uart.h"
#include "shell.h"

#define PM_PASSWORD 0x5a000000
#define PM_RSTC ((volatile unsigned int*)0x3F10001c)
#define PM_WDOG ((volatile unsigned int*)0x3F100024)

void main() {
  // set up serial console
  uart_init();
  uart_flush();

  char *osdi = "OSDI Welcome\n";
  uart_puts(osdi);

  for(;;) {
    put_shell();
  }
}
