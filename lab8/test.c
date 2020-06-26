#include "sys.h"
#include "vfs.h"
#include "uart.h"

void test_init(void)
{
  char buf[512+256];
  int size;

  uart_puts("open /HELLO\n");
  struct vfs_file_struct * hello_file = vfs_open("/HELLO", 0);
  if(hello_file == 0)
  {
    uart_puts("Assert hello_file != 0\n");
    while(1);
  }
  uart_puts("---\n");

  uart_puts("read /HELLO\n");
  size = vfs_read(hello_file, buf, 100);
  buf[size++] = '\0';
  uart_puts(buf);
  uart_puts("\n---\n");
  vfs_close(hello_file);

  uart_puts("open /WORLD\n");
  struct vfs_file_struct * world_file = vfs_open("/WORLD", 0);
  if(hello_file == 0)
  {
    uart_puts("Assert world_file != 0\n");
    while(1);
  }
  uart_puts("---\n");

  uart_puts("read /WORLD\n");
  size = vfs_read(world_file, buf, 100);
  buf[size++] = '\0';
  uart_puts(buf);
  uart_puts("\n---\n");
  vfs_close(world_file);

  return;
}
