#include "shell.h"

void main() {
  // set up serial console
  uart_init();
  uart_flush();
  char *osdi = "OSDI Welcome\n";
  uart_puts(osdi);

  /*
  struct page* p0, *p1;
  p0 = page_alloc();
  p1 = page_alloc();
  page_free(p0);
  p0 = page_alloc();
  */

  init_buddy_system();
  struct block* m1 = get_space(3 << 12);
  struct block* m2 = get_space(5 << 12);
  struct block* m3 = get_space(5 << 12);
  struct block* m4 = get_space(5 << 12);
  struct block* m5 = get_space(5 << 12);

  free_space(m1);
  free_space(m5);

  /*
  for(;;) {
    put_shell();
  }
  */
}
