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

  uart_puts("open & create /WHITEBOA.RD\n");
  struct vfs_file_struct * whiteboard_file = vfs_open("/WHITEBOA.RD", O_CREAT);
  if(whiteboard_file == 0)
  {
    uart_puts("Assert whiteboard_file != 0\n");
    while(1);
  }
  uart_puts("---\n");

  uart_puts("write /WHITEBOA.RD with tragedy of Darth Plagueis\n");
  //size = vfs_write(whiteboard_file, "Roses are red, violets are blue. Unregistered Hypercam 2\n", 57);
  size = vfs_write(whiteboard_file, "Did you ever hear the tragedy of Darth Plagueis The Wise? I thought not. It's not a story the Jedi would tell you. It's a Sith legend. Darth Plagueis was a Dark Lord of the Sith, so powerful and so wise he could use the Force to influence the midichlorians to create life… He had such a knowledge of the dark side that he could even keep the ones he cared about from dying. The dark side of the Force is a pathway to many abilities some consider to be unnatural. He became so powerful… the only thing he was afraid of was losing his power, which eventually, of course, he did. Unfortunately, he taught his apprentice everything he knew, then his apprentice killed him in his sleep. Ironic. He could save others from death, but not himself.\n", 745);
  uart_puts("---\n");

  uart_puts("read /WHITEBOA.RD\n");
  size = vfs_read(whiteboard_file, buf, 999);
  buf[size++] = '\0';
  uart_puts(buf);
  uart_puts("---\n");
  vfs_close(whiteboard_file);

  return;
}
