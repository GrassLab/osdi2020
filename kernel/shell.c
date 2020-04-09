#include <uart.h>
#include <string.h>
#include <mm.h>
#include <mbox.h>
#include <lfb.h>
#include <timer.h>
#include "shell.h"
#include "irq.h"

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
	  uart_puts ("exc - raise exception (svc #1)\n");
	  uart_puts ("irq - trigger timer interrupt\n");
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
      else if (!strncmp ("picture", buf, 7))
	{
	  picture (&buf[8]);
	}
      else if (!strncmp ("loadimg", buf, 7))
	{
	  loadimg (strtol (buf + 7, 0, 16));
	}
      else if (!strcmp ("exc", buf))
	{
	  asm volatile ("svc #1");
	}
      else if (!strcmp ("irq", buf))
	{
	  core_timer_enable ();
	  local_timer_init ();
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
  extern void *__start_bootloader;
  extern void *__stop_bootloader;
  unsigned long __bootloader_size =
    (unsigned long) &__stop_bootloader - (unsigned long) &__start_bootloader;
  unsigned long img_size;
  unsigned long rebased_bootloader;
  unsigned long rebased_end;
  unsigned long rebased_loadimg;
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
  // rebase tiny bootloader
  rebased_bootloader = address + img_size;
  rebased_end = rebased_bootloader + __bootloader_size;
  if ((unsigned long) &__stop_bootloader > rebased_end)
    rebased_bootloader = (unsigned long) &__stop_bootloader;
  // place new bootloader
  memcpy ((char *) rebased_bootloader, &__start_bootloader,
	  __bootloader_size);
  // call rebased bootloader
  rebased_loadimg =
    rebased_bootloader + ((unsigned long) &loadimg_jmp -
			  (unsigned long) &__start_bootloader);
  asm volatile ("mov x0, %0\n" "mov x1, %1\n" "mov sp, %2\n"
		"blr %3\n"::"r" (address), "r" (img_size),
		"r" ((rebased_end + 0x8000) & ~0xf),
		"r" (rebased_loadimg):"x0", "x1");
  //((void (*)(void *, unsigned long, void *)) rebased_bootloader)((void *) address, img_size, rebased_bootloader + &__bootloader_size);
}

void
loadimg_jmp (void *address, unsigned long img_size)
{
  // save image base
  uart_read ((char *) address, img_size);
  ((void (*)(void)) (address)) ();
}

void
picture (char buf[3])
{
  lfb_showpicture (buf);
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

void
exc (unsigned long elr_el2, unsigned long esr_el2)
{
  printf ("Exception return address 0x%p\r\n", (void *) elr_el2);
  printf ("Exception class (EC) 0x%x\r\n", esr_el2 >> 26);
  printf ("Instruction specific syndrome (ISS) 0x%x\r\n", esr_el2 & 0xffffff);
}
