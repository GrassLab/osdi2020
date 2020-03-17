#include "uart.h"
#include "gpio.h"

void
uart_init ()
{
  // set GPIO14 and GPIO15 to alternate function 5 (010)
  SET_FSEL (1, 4, 2);
  SET_FSEL (1, 5, 2);
  *GPPUD = 0;			// disable pull up/down
  WAIT_CONTROL_SIG;
  *GPPUDCLK0 = (1 << 14) | (1 << 15);
  WAIT_CONTROL_SIG;
  *GPPUDCLK0 = 0;		// flush GPIO setup

  /* initialize UART */
  *AUX_ENABLE |= 1;		// enable UART1, AUX mini uart
  *AUX_MU_CNTL = 0;
  *AUX_MU_IER = 0;
  *AUX_MU_LCR = 3;		// 8 bits
  *AUX_MU_MCR = 0;
  *AUX_MU_BAUD = 270;		// 115200 baud
  *AUX_MU_IIR = 0x6;		// disable interrupts
  *AUX_MU_CNTL = 3;
}

void
uart_send (unsigned int c)
{
  /* wait until we can send */
  do
    {
      asm volatile ("nop");
    }
  while (!(*AUX_MU_LSR & 0x20));
  /* write the character to the buffer */
  *AUX_MU_IO = c;
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

unsigned char
uart_getc ()
{
  char r;
  /* wait until something is in the buffer */
  do
    {
      asm volatile ("nop");
    }
  while (!(*AUX_MU_LSR & 0x01));
  /* read it and return */
  r = (char) (*AUX_MU_IO);
  /* convert carrige return to newline */
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
	  ++cnt;
	  break;
	}
    }
  buf[cnt - 1] = 0;
  return cnt;
}
