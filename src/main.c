#include "shell.h"

void main() {
  // set up serial console
  uart_init();
  uart_flush();
  init_buddy_system();
  char *osdi = "OSDI Welcome\n";
  uart_puts(osdi);
  //uart_getc();

  init_rootfs();
  //uart_puts(osdi);
  fat32_init();

  /*
  char buf[100];
  struct file* a = vfs_open("hello", 0);
  a = vfs_open("hello", O_CREAT);
  struct file* b = vfs_open("world", O_CREAT);
  vfs_write(a, "Hello ", 6);
  vfs_write(b, "World!", 6);
  vfs_close(a);
  vfs_close(b);
  b = vfs_open("hello", 0);
  a = vfs_open("world", 0);
  int sz;
  sz = vfs_read(b, buf, 100);
  sz += vfs_read(a, buf + sz, 100);
  buf[sz] = '\0';
  uart_puts(buf);
  uart_puts("\n");
  vfs_ls("/");
  */

  for(;;) {
    put_shell();
  }
}
