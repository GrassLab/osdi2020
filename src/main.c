#include "uart.h"
#include "shell.h"

void main() {
  // set up serial console
  uart_init();
  uart_flush();

  lfb_init();
  // display a pixmap
  lfb_showpicture();
  char *osdi = "OSDI Welcome\n";
  uart_puts(osdi);

  for(;;) {
    put_shell();
  }
}
