#include <stdlib.h>
#define BUF_SIZE 0x30

void
read_command (char *buf, size_t len)
{
  char c;
  size_t i;
  for (i = 0; i < len - 1; ++i)
    {
      uart_read (&c, 1);
      uart_write (&c, 1);
      if (c == '\r')
	{
	  c = '\n';
	  uart_write (&c, 1);
	  break;
	}
      buf[i] = c;
    }
  buf[i] = '\0';
}

int
main ()
{
  char buf[BUF_SIZE];
  while (1)
    {
      uart_write ("# ", 2);
      read_command (buf, BUF_SIZE);
      printf ("%s: command not found\r\n", buf);
    }
  return 0;
}
