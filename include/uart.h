#ifndef _UART_H_
#define _UART_H_

#include "gpio.h"



void uart_init();
void uart_send(unsigned int c);
char uart_getc();
int  ReadLine(char *buf);
void uart_puts(char *s);
void uart_hex(unsigned int d);

#endif
