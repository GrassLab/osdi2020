#ifndef UART_H
#define UART_H

#include "gpio.h"

/* PL011 UART registers */
#define UART0_DR        MMIO_BASE+0x00201000// ((volatile unsigned int*)(MMIO_BASE+0x00201000))
#define UART0_FR        MMIO_BASE+0x00201018// ((volatile unsigned int*)(MMIO_BASE+0x00201018))
#define UART0_IBRD      MMIO_BASE+0x00201024// ((volatile unsigned int*)(MMIO_BASE+0x00201024))
#define UART0_FBRD      MMIO_BASE+0x00201028// ((volatile unsigned int*)(MMIO_BASE+0x00201028))
#define UART0_LCRH      MMIO_BASE+0x0020102C// ((volatile unsigned int*)(MMIO_BASE+0x0020102C))
#define UART0_CR        MMIO_BASE+0x00201030// ((volatile unsigned int*)(MMIO_BASE+0x00201030))
#define UART0_IMSC      MMIO_BASE+0x00201038// ((volatile unsigned int*)(MMIO_BASE+0x00201038))
#define UART0_ICR       MMIO_BASE+0x00201044// ((volatile unsigned int*)(MMIO_BASE+0x00201044))
#define UART0_MIS       MMIO_BASE+0x00201040// ((volatile unsigned int*)(MMIO_BASE+0x00201044))
#define IRQ_ENABLE_REGISTER_2       MMIO_BASE+0xb214

void uart_init();
void uart_send(unsigned int c);
char uart_recv_char();
char uart_getc();
int uart_buffer_empty();
void uart_save(char c);
void uart_get_string(char *str);
void uart_puts(char *s);
void uart_print(char *s);
void uart_print_int(int num);
void uart_print_hex(unsigned int num);
void uart_print_double(double num);
void print_uart_clock();
void putc(void *p, char c);

#endif