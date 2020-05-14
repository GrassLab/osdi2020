#include <gpio.h>
#include "uart.h"
#include "mbox.h"

void
uart_init ()
{
  /* initialize UART */
  *UART0_CR = 0;		// turn off UART0

  /* set up clock for consistent divisor values */
  mbox[0] = 9 * 4;
  mbox[1] = MBOX_REQUEST;
  mbox[2] = MBOX_TAG_SETCLKRATE;	// set clock rate
  mbox[3] = 12;
  mbox[4] = 8;
  mbox[5] = 2;			// UART clock
  mbox[6] = 4000000;		// 4Mhz
  mbox[7] = 0;			// clear turbo
  mbox[8] = MBOX_TAG_LAST;
  mbox_call (MBOX_CH_PROP);

  // set GPIO14 and GPIO15 to alternate function 0 (100)
  SET_FSEL (1, 4, 4);
  SET_FSEL (1, 5, 4);
  *GPPUD = 0;			// disable pull up/down
  WAIT_CONTROL_SIG;
  *GPPUDCLK0 = (1 << 14) | (1 << 15);
  WAIT_CONTROL_SIG;
  *GPPUDCLK0 = 0;		// flush GPIO setup

  *UART0_ICR = 0x7FF;		// clear interrupts
  *UART0_IBRD = 2;		// 115200 baud
  *UART0_FBRD = 0xB;
  *UART0_LCRH = 3 << 5;		// 8n1
  *UART0_CR = 0x301;		// enable Tx, Rx, FIFO
}

void
uart_send (char c)
{
  /* wait until we can send */
  do
    {
      asm volatile ("nop");
    }
  while (*UART0_FR & 0x20);
  /* write the character to the buffer */
  *(char *) UART0_DR = c;
}

void
uart_puts (char *s)
{
  while (*s)
    {
      /* convert newline to carrige return + newline */
      if (*s == '\n')
	uart_send ('\r');
      uart_send (*s++);
    }
}

char
uart_getc ()
{
  char r;
  /* wait until something is in the buffer */
  do
    {
      asm volatile ("nop");
    }
  while (*UART0_FR & 0x10);
  /* read it and return */
  r = (char) (*UART0_DR);
  return r;
}

int
uart_readline (int size, char *buf)
{
  int cnt;
  for (cnt = 0; cnt < size; ++cnt)
    {
      buf[cnt] = uart_getc ();
      uart_send (buf[cnt]);
      if (buf[cnt] == '\n')
	{
	  uart_send ('\r');
	  ++cnt;
	  break;
	}
    }
  buf[cnt - 1] = 0;
  return cnt;
}

void
uart_hex (unsigned int d)
{
  unsigned int n;
  int c;
  for (c = 28; c >= 0; c -= 4)
    {
      // get highest tetrad
      n = (d >> c) & 0xF;
      // 0-9 => '0'-'9', 10-15 => 'A'-'F'
      n += n > 9 ? 0x37 : 0x30;
      uart_send (n);
    }
}

void
_putchar (char character)
{
  uart_send (character);
}

size_t
do_uart_read (char *buf, size_t count)
{
  size_t i;
  for (i = 0; i < count; ++i)
    *buf++ = uart_getc ();
  return i;
}

size_t
sys_uart_read (char *buf, size_t count)
{
  return do_uart_read (buf, count);
}

size_t
do_uart_write (char *buf, size_t size)
{
  size_t i;
  for (i = 0; i < size; ++i)
    uart_send (*buf++);
  return i;
}

size_t
sys_uart_write (char *buf, size_t size)
{
  return do_uart_write (buf, size);
}
