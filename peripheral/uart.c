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
  // enable interrupt
  *UART0_IMSC = 3 << 4;		// Tx, Rx
  // init uart buf
  read_buf.head = 0;
  read_buf.tail = 0;
  write_buf.head = 0;
  write_buf.tail = 0;
}

void
uart_send (char c)
{
  char r;
  if (*UART0_FR & 0x80)
    {
      // we need to send one character to trigger interrupt.
      // because the interrupt only set after data transmitted
      if (QUEUE_EMPTY (write_buf))
	{
	  *UART0_DR = c;
	}
      else
	{
	  r = QUEUE_GET (write_buf);
	  QUEUE_POP (write_buf);
	  QUEUE_SET (write_buf, c);
	  QUEUE_PUSH (write_buf);
	  *UART0_DR = r;
	}
    }
  else
    {
      // Raspberry PI is toooooo slow
      // We need push the data into queue
      if (!QUEUE_FULL (write_buf))
	{
	  QUEUE_SET (write_buf, c);
	  QUEUE_PUSH (write_buf);
	}
      // else: drop that :(
    }
  return;
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

  while (QUEUE_EMPTY (read_buf))
    asm volatile ("wfi");
  r = QUEUE_GET (read_buf);
  QUEUE_POP (read_buf);
  return r == '\r' ? '\n' : r;
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

void
uart_read (char *buf, unsigned long count)
{
  while (count--)
    *buf++ = uart_getc ();
}
