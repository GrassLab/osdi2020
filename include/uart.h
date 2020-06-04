#ifndef _UART_H
#define _UART_H

#include "gpio.h"
#include "mbox.h"

void uart_init();
void uart_send(unsigned int c);
char uart_getc();
char uart_getb();
void uart_puts(char *s);
void uart_hex(unsigned int d);
void uart_flush();

#endif
