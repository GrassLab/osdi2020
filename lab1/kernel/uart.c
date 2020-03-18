#include "io.h"
/**
 * Enable GPIO & set baud rate
 */
void uart_init()
{
	int r;
	*AUX_ENABLE |= 1;
	*AUX_MU_CNTL = 0;
	*AUX_MU_LCR = 3;
	*AUX_MU_MCR = 0;
	*AUX_MU_IER = 0;
	*AUX_MU_IIR = 0x6;
	*AUX_MU_BAUD = 270;
	r = *GPFSEL1;
	r &= ~((7 << 12) | (7 << 15));
	r |= (2 << 12) | (2 << 15);
	*GPFSEL1 = r;
	*GPPUD = 0;
	*GPPUDCLK0 = (1 << 14) | (1 << 15);
	*GPPUDCLK0 = 0;
	*AUX_MU_CNTL = 3;
}
/**
 * Send char
 */
void send(unsigned int c)
{
	if (c > 127)
		return;
	while (!(*AUX_MU_LSR & 0x20))
		;
	*AUX_MU_IO = c;
}
/**
 * Get char
 */
char uart_getc()
{
	char r;
	while (!(*AUX_MU_LSR & 0x01))
		;
	r = (char)(*AUX_MU_IO);
	if (r > 127)
		return 0x0;
	return r == '\r' ? '\n' : r;
}

/**
 * Send string
 */
void uart_puts(char *s)
{
	while (*s) {
		if (*s == '\n')
			send('\r');
		send(*s++);
	}
}
