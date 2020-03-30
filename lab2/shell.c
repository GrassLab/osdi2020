#include "shell.h"
#include "uart.h"
#include "string.h"
#include "mm.h"
#include "printf.h"
#include "mbox.h"
#include "lfb.h"

#define PM_PASSWORD   0x5a000000
#define PM_RSTC       ((volatile unsigned int*)(MMIO_BASE+0x0010001c))
#define PM_WDOG       ((volatile unsigned int*)(MMIO_BASE+0x00100024))

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
	  uart_puts ("reboot - reboot\n");
	  uart_puts
	    ("hardware - show board revision and VC Core base address\n");
	  uart_puts ("picture - show picture\n");
	}
      else if (!strcmp ("hello", buf))
	{
	  uart_puts ("Hello World!\n");
	}
      else if (!strcmp ("timestamp", buf))
	{
	  print_time ();
	}
      else if (!strcmp ("reboot", buf))
	{
	  reset (10);
	}
      else if (!strncmp ("hardware", buf, 9))
	{
	  hardware ();
	}
      else if (!strncmp ("picture", buf, 8))
	{
	  picture ();
	}
      else
	{
	  uart_puts (buf);
	  uart_puts (": command not found\n");
	}
    }
}

void
picture ()
{
  lfb_showpicture ();
}

void
hardware ()
{
  char *buf;
  unsigned int base_addr;
  unsigned int revision;
  struct mbox_msg *msg;
  msg = (struct mbox_msg *) &mbox;

  // set tag
  msg->tag.id = 0x00010002;
  msg->tag.buf_size = 4;
  msg->tag.code = 0;
  // set message
  msg->buf_size = sizeof (struct mbox_msg) + msg->tag.buf_size + 4;
  msg->code = 0;
  // set tag buf and tag end
  bzero (msg + 1, msg->tag.buf_size + 4);
  mbox_call (MBOX_CH_PROP);

  revision = *(unsigned int *) (msg + 1);

  // set tag
  msg->tag.id = 0x00010006;
  msg->tag.buf_size = 8;
  msg->tag.code = 0;
  // set mseeage
  msg->buf_size = sizeof (struct mbox_msg) + msg->tag.buf_size + 4;
  msg->code = 0;
  // set tag buf and tag end
  bzero (msg + 1, msg->tag.buf_size + 4);
  mbox_call (MBOX_CH_PROP);

  base_addr = *(unsigned int *) (msg + 1);

  printf ("board revision: %x\r\n", revision);
  printf ("VC Core base address: %x\r\n", base_addr);
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
		"mrs %1, CNTPCT_EL0\n":"=r" (freq), "=r" (cnt));
  result = (double) cnt / (double) freq;
  printf ("[%lf]\r\n", result);
}

void
reset (int tick)
{
  // reboot after watchdog timer expire
  *PM_RSTC = PM_PASSWORD | 0x20;	// full reset
  *PM_WDOG = PM_PASSWORD | tick;	// number of watchdog tick
}

void
cancel_reset ()
{
  *PM_RSTC = PM_PASSWORD | 0;	// full reset
  *PM_WDOG = PM_PASSWORD | 0;	// number of watchdog tick
}
