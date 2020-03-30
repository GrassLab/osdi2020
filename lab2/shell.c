#include "shell.h"
#include "uart.h"
#include "string.h"
#include "mm.h"
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
	  uart_puts ("loadimg [address] - load image to address\n");
	}
      else if (!strcmp ("hello", buf))
	{
	  uart_puts ("Hello World!\n");
	}
      else if (!strcmp ("timestamp", buf))
	{
	  ftoa (get_time (), buf);
	  uart_puts ("[");
	  uart_puts (buf);
	  uart_puts ("]\n");
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
      else if (!strncmp ("loadimg", buf, 7))
	{
	  loadimg (strtol (buf + 7, 0, 16));
	}
      else
	{
	  uart_puts (buf);
	  uart_puts (": command not found\n");
	}
    }
}

void
loadimg (unsigned long address)
{
  unsigned img_size;
  char buf[0x20];
  uart_puts ("Load image into 0x");
  uart_hex (address);
  uart_puts ("\n");
  uart_puts ("Please give me the image.\n");
  uart_read ((char *) &img_size, 4);
  ftoa (get_time (), buf);
  uart_puts ("[");
  uart_puts (buf);
  uart_puts ("] image size: 0x");
  uart_hex (img_size);
  uart_puts ("\n");
  uart_read ((char *) address, img_size);
  ((void (*)(void)) address) ();
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

  uart_puts ("board revision: ");
  uart_hex (revision);
  uart_puts ("\n");

  uart_puts ("VC Core base address: ");
  uart_hex (base_addr);
  uart_puts ("\n");
}

void
ftoa (double val, char *buf)
{
  double scan = 1000000;
  int cnt = 0;
  int met_num = 0;

  while (cnt < 6)
    {
      *buf = (long) (val / scan) % 10;
      if (*buf != 0)
	met_num = 1;
      *buf += '0';
      if (met_num)
	buf++;
      if (scan < 1)
	cnt++;
      if (scan == 1)
	*buf++ = '.';
      scan /= 10;
    }
  *buf = 0;
}

double
get_time ()
{
  unsigned long freq;
  unsigned long cnt;
  double result;
  char buf[0x20];
  int len = 0;

  asm volatile ("mrs %0, CNTFRQ_EL0\n"
		"mrs %1, CNTPCT_EL0\n":"=r" (freq), "=r" (cnt));
  return (double) cnt / (double) freq;
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
