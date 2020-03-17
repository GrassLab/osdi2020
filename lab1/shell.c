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
	}
      else if (!strcmp ("hello", buf))
	{
	  uart_puts ("Hello World!\n");
	}
      else
	{
	  uart_puts (buf);
	  uart_puts (": command not found\n");
	}
    }
}
