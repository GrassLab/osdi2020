#include "io.h"
#include "uart.h"
/* PL011 UART registers */
#define UART0_DR ((volatile unsigned int *)(MMIO_BASE + 0x00201000))
#define UART0_FR ((volatile unsigned int *)(MMIO_BASE + 0x00201018))
#define UART0_IBRD ((volatile unsigned int *)(MMIO_BASE + 0x00201024))
#define UART0_FBRD ((volatile unsigned int *)(MMIO_BASE + 0x00201028))
#define UART0_LCRH ((volatile unsigned int *)(MMIO_BASE + 0x0020102C))
#define UART0_CR ((volatile unsigned int *)(MMIO_BASE + 0x00201030))
#define UART0_IMSC ((volatile unsigned int *)(MMIO_BASE + 0x00201038))
#define UART0_ICR ((volatile unsigned int *)(MMIO_BASE + 0x00201044))
#define UART0_IFLS ((volatile unsigned int *)(MMIO_BASE + 0x00201034))
#define ENABLE_IRQ2 (unsigned int *)(MMIO_BASE + 0xb214)

#define INTR_ENABLE 0
// struct ring_buf recv_buf, trans_buf;
/**
 * Enable GPIO & set baud rate
 */
void uart_init()
{
	int r;
	recv_buf.head = 0;
	recv_buf.tail = 0;
	trans_buf.head = 0;
	trans_buf.tail = 0;
	*UART0_CR = 0;
	uint32_t __attribute__((aligned(16)))
	mailbox[9] = { 9 * 4, MBOX_REQUEST, 0x00038002, 12,	      8,
		       2,     4000000,	    0,		MBOX_TAG_LAST };
	mbox_send(MBOX_CH_PROP, mailbox);
	// int res = mbox_read(MBOX_CH_PROP, mailbox);
	r = *GPFSEL1;
	r &= ~((7 << 12) | (7 << 15));
	r |= (4 << 12) | (4 << 15);
	*GPFSEL1 = r;
	*GPPUD = 0;
	r = 150;
	do {
		asm volatile("nop");
	} while (r--);
	*GPPUDCLK0 = (1 << 14) | (1 << 15);
	r = 150;
	do {
		asm volatile("nop");
	} while (r--);
	*GPPUDCLK0 = 0;
	*UART0_ICR = 0x7FF; // clear interrupts
	*UART0_IBRD = 2; // integer 115200 baud
	*UART0_FBRD = 0xB; // fractional boud rate divisor
	*UART0_LCRH = 0b11 << 5; // 8n1
	if (INTR_ENABLE) {
		*ENABLE_IRQ2 = 1 << 25;
		*UART0_IFLS = 0b100001;
		*UART0_IMSC = 0b111 << 4;
	}
	*UART0_CR = 0x301; // enable Tx, Rx, FIFO
}

inline int ring_buf_full(struct ring_buf *buf)
{
	return buf->head == (buf->tail + 1) % BUF_SIZE;
}

inline int ring_buf_empty(struct ring_buf *buf)
{
	return buf->head == buf->tail;
}
/**
 * Send char
 */
// void __attribute__((section(".bootloader"))) send(unsigned int c)
void send(unsigned int c)
{
	if (INTR_ENABLE) {
		// *UART0_DR = c;
		// if (*UART0_FR & 0x20) {
		// 	ring_buf_put(&trans_buf, c);
		// } else {
		// 	*UART0_DR = c;
		// }
		if (!(*UART0_FR & 0x20)) {
			if (ring_buf_empty(&trans_buf)) {
				*UART0_DR = c;
			} else {
				ring_buf_put(&trans_buf, c);
				*UART0_DR = ring_buf_get(&trans_buf);
			}
		} else {
			/* do nothing for now */
			if (!ring_buf_full(&trans_buf)) {
				ring_buf_put(&trans_buf, c);
			}
		}
	} else {
		do {
			asm volatile("nop");
		} while (*UART0_FR & 0x20);
		*UART0_DR = c;
	}
}

void ring_buf_put(struct ring_buf *buf, uint8_t data)
{
	if (ring_buf_full(buf)) { // full
		return;
	}
	buf->tail = (buf->tail + 1) % BUF_SIZE;
	buf->_buf[buf->tail] = data;
}
char ring_buf_get(struct ring_buf *buf)
{
	if (ring_buf_empty(buf)) { // empty
		return -1;
	}
	buf->head = (buf->head + 1) % BUF_SIZE;
	char r = buf->_buf[buf->head];
	return r;
}
/**
 * Send string
 */
void uart_puts(char *s)
{
	while (*s) {
		if (*s == '\n') {
			send('\r');
		}
		send(*s++);
	}
}
/**
 * Get char
 */
// char __attribute__((section(".bootloader"))) uart_getc()
char uart_getc()
{
	char r;
	if (INTR_ENABLE) {
		while (ring_buf_empty(&recv_buf)) {
			asm volatile("wfi");
		}
		r = ring_buf_get(&recv_buf);
	} else {
		do {
			asm volatile("nop");
		} while (*UART0_FR & 0x10);
		r = (char)(*UART0_DR);
	}
	return r == '\r' ? '\n' : r;
}

void uart_hex(unsigned long long int d)
{
	unsigned long long int n;
	int bits = d == ((int)d) ? 32 : 64;
	int c;
	for (c = bits - 4; c >= 0; c -= 4) {
		// get highest tetrad
		n = (d >> c) & 0xF;
		// 0-9 => '0'-'9', 10-15 => 'A'-'F'
		n += n > 9 ? 0x37 : 0x30;
		send(n);
	}
}

// void *memcpy(void *dest, const void *src, uint32_t len)
// {
// 	char *d = dest;
// 	const char *s = src;
// 	while (len--)
// 		*d++ = *s++;
// 	return dest;
// }