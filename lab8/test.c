#include "sys.h"
#include "vfs.h"
#include "uart.h"

void test_init(void)
{
  char buf[896];
  int size;

  uart_puts("open /HELLO.TXT\n");
  struct vfs_file_struct * hello_file = vfs_open("/HELLO.TXT", 0);
  if(hello_file == 0)
  {
    uart_puts("Assert hello_file != 0\n");
    while(1);
  }
  uart_puts("---\n");

  uart_puts("read /HELLO.TXT\n");
  size = vfs_read(hello_file, buf, 100);
  buf[size++] = '\0';
  uart_puts(buf);
  uart_puts("\n---\n");
  vfs_close(hello_file);

  uart_puts("open /PLAGUEIS\n");
  struct vfs_file_struct * plagueis_file = vfs_open("/PLAGUEIS", 0);
  if(plagueis_file == 0)
  {
    uart_puts("Assert plagueis_file != 0\n");
    while(1);
  }
  uart_puts("---\n");

  uart_puts("read /PLAGUEIS\n");
  size = vfs_read(plagueis_file, buf, 999);
  buf[size++] = '\0';
  uart_puts(buf);
  uart_puts("---\n");
  vfs_close(plagueis_file);

  uart_puts("open /WHITEBOA.RD\n");
  struct vfs_file_struct * whiteboard_file = vfs_open("/WHITEBOA.RD", 0);
  if(whiteboard_file == 0)
  {
    uart_puts("Assert whiteboard_file != 0\n");
    while(1);
  }
  uart_puts("---\n");

  uart_puts("write /WHITEBOA.RD with meme\n");
  size = vfs_write(whiteboard_file, "Roses are red, violets are blue. Unregistered Hypercam 2\n", 57);
  uart_puts("---\n");

  uart_puts("read /WHITEBOA.RD\n");
  size = vfs_read(whiteboard_file, buf, 999);
  buf[size++] = '\0';
  uart_puts(buf);
  uart_puts("---\n");
  vfs_close(whiteboard_file);

  return;
}
