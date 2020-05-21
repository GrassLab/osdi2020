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
	r = 150;
	while (r-- > 0)
		;
	*GPPUDCLK0 = (1 << 14) | (1 << 15);
	r = 150;
	while (r-- > 0)
		;
	*GPPUDCLK0 = 0;
	*AUX_MU_CNTL = 3;
}
/**
 * Send char
 */
void send(unsigned int c)
{
	// if (c > 126)
	// 	return;
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
	// if (r > 126)
	// 	return 0x0;
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

void uart_hex(unsigned long long int d)
{
	unsigned long long int n;
	int c;
	for (c = 60; c >= 0; c -= 4) {
		// get highest tetrad
		n = (d >> c) & 0xF;
		// 0-9 => '0'-'9', 10-15 => 'A'-'F'
		n += n > 9 ? 0x37 : 0x30;
		send(n);
	}
}

void ring_buf_put(struct ring_buf *buf, uint8_t data)
{
}
char ring_buf_get(struct ring_buf *buf)
{
	return 0;
}

inline int ring_buf_full(struct ring_buf *buf)
{
	return 0;
}

inline int ring_buf_empty(struct ring_buf *buf)
{
	return 0;
}