#include "io.h"

/* PL011 UART registers */
#define UART0_DR ((volatile unsigned int *)(MMIO_BASE + 0x00201000))
#define UART0_FR ((volatile unsigned int *)(MMIO_BASE + 0x00201018))
#define UART0_IBRD ((volatile unsigned int *)(MMIO_BASE + 0x00201024))
#define UART0_FBRD ((volatile unsigned int *)(MMIO_BASE + 0x00201028))
#define UART0_LCRH ((volatile unsigned int *)(MMIO_BASE + 0x0020102C))
#define UART0_CR ((volatile unsigned int *)(MMIO_BASE + 0x00201030))
#define UART0_IMSC ((volatile unsigned int *)(MMIO_BASE + 0x00201038))
#define UART0_ICR ((volatile unsigned int *)(MMIO_BASE + 0x00201044))

/**
 * Enable GPIO & set baud rate
 */
void uart_init()
{
	int r;
	*UART0_CR = 0;
	uint32_t __attribute__((aligned(16)))
	mailbox[9] = { 9 * 4, MBOX_REQUEST, 0x00038002, 12,	      8,
		       2,     4000000,	    0,		MBOX_TAG_LAST };
	mbox_send(MBOX_CH_PROP, mailbox);
	int res = mbox_read(MBOX_CH_PROP, mailbox);
	r = *GPFSEL1;
	r &= ~((7 << 12) | (7 << 15));
	r |= (4 << 12) | (4 << 15);
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
	*UART0_ICR = 0x7FF; // clear interrupts
	*UART0_IBRD = 2; // 115200 baud
	*UART0_FBRD = 0xB;
	*UART0_LCRH = 0b11 << 5; // 8n1
	*UART0_CR = 0x301; // enable Tx, Rx, FIFO
}

/**
 * Send char
 */
void __attribute__((section(".bootloader"))) send(unsigned int c)
{
	// if (c > 126)
	// 	return;
	while (*UART0_FR & 0x20)
		;
	*UART0_DR = c;
}
/**
 * Get char
 */
char __attribute__((section(".bootloader"))) uart_getc()
{
	char r;
	while (*UART0_FR & 0x10)
		;
	r = (char)(*UART0_DR);
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
