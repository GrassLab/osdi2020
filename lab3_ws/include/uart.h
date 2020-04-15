#ifndef _UART_INCLUDE_H_
#define _UART_INCLUDE_H_

#include "stdint.h"

struct ring_buf {
	uint8_t _buf[16];
	uint8_t head;
	uint8_t tail;
};
struct ring_buf recv_buf, trans_buf;
void ring_buf_put(struct ring_buf buf, uint8_t data);
char ring_buf_get(struct ring_buf buf);

void uart_init();
char uart_getc();
void uart_puts(char *s);
void uart_hex(unsigned long long int d);
void send(unsigned int c);
#endif