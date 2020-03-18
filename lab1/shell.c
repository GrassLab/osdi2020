#include "shell.h"
#include "uart.h"
#include "string.h"

void
shell_interactive ()
{
  char buf[CMD_SIZE];

  while (1)
    {
      uart_puts ("# ");
      uart_readline (CMD_SIZE, buf);
      if (!strcmp ("help", buf))
	{
	  uart_puts ("help - show command list\n");
	  uart_puts ("hello - say hello\n");
	  uart_puts ("timestamp - get current timestamp\n");
	}
      else if (!strcmp ("hello", buf))
	{
	  uart_puts ("Hello World!\n");
	}
      else if (!strcmp ("timestamp", buf))
	{
	  print_time ();
	}
      else
	{
	  uart_puts (buf);
	  uart_puts (": command not found\n");
	}
    }
}

void
ftoa (double f, int size, char *buf)
{
  double probe = 1000000;
  int cnt = 0;

  while (f / probe < 1)
    {
      if (probe == 1)
        break;
      probe /= 10;
    }
  while (cnt < size - 1)
    {
      buf[cnt++] = ((char) (f / probe) % 10) + '0';
      if (probe == 1)
        buf[cnt++] = '.';
      probe /= 10;
    }
  buf[size - 1] = '\0';
}

void
print_time ()
{
  unsigned long freq;
  unsigned long cnt;
  double result;
  char buf[0x20];
  int len = 0;

  asm volatile ("mrs %0, CNTFRQ_EL0\n"
		"mrs %1, CNTPCT_EL0\n"
		: "=r" (freq), "=r" (cnt));
  result = (double) cnt / (double) freq;
  ftoa (result, 0x20, buf);
  uart_puts ("[");
  uart_puts (buf);
  uart_puts ("]\n");
}
