#ifndef _UART_H_
#define _UART_H_
#include "gpio.h"

/* PL011 UART registers */
#define UART0_DR ((volatile unsigned int *)(MMIO_BASE + 0x00201000))
#define UART0_FR ((volatile unsigned int *)(MMIO_BASE + 0x00201018))
#define UART0_IBRD ((volatile unsigned int *)(MMIO_BASE + 0x00201024))
#define UART0_FBRD ((volatile unsigned int *)(MMIO_BASE + 0x00201028))
#define UART0_LCRH ((volatile unsigned int *)(MMIO_BASE + 0x0020102C))
#define UART0_CR ((volatile unsigned int *)(MMIO_BASE + 0x00201030))
#define UART0_IMSC ((volatile unsigned int *)(MMIO_BASE + 0x00201038))
#define UART0_ICR ((volatile unsigned int *)(MMIO_BASE + 0x00201044))

void uart_init();
void uart_send(unsigned int c);
char uart_recv() __attribute__((section(".bootloader")));
char uart_getc() __attribute__((section(".bootloader")));
void uart_puts(char *s);

void uart_send_hex(unsigned int);

int uart_gets(char *buf, int buf_size);
#endif
