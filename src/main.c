#include "shell.h"

void main() {
  // set up serial console
  uart_init();
  uart_flush();
  page_init();
  //lfb_init();
  // display a pixmap
  //lfb_showpicture();
  char *osdi = "OSDI Welcome\n";
  uart_puts(osdi);



  //asm("svc 2");
  //create_tasks();


  /*
  for(;;) {
    put_shell();
  }
  */
}
