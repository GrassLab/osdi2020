/*
 * Copyright (C) 2018 bzt (bztsrc@github)
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */

/* PL011 UART registers */
#include "config.h"

// #define UART0_DR        ((MMIO_BASE+0x00201000))
// #define UART0_FR        ((MMIO_BASE+0x00201018))
// #define UART0_IBRD      ((MMIO_BASE+0x00201024))
// #define UART0_FBRD      ((MMIO_BASE+0x00201028))
// #define UART0_LCRH      ((MMIO_BASE+0x0020102C))
// #define UART0_CR        ((MMIO_BASE+0x00201030))
// #define UART0_IMSC      ((MMIO_BASE+0x00201038))
// #define UART0_ICR       ((MMIO_BASE+0x00201044))


#define UART0_DR        ((MMIO_BASE+0x00201000))
#define UART0_FR        ((MMIO_BASE+0x00201018))
#define UART0_IBRD      ((MMIO_BASE+0x00201024))
#define UART0_FBRD      ((MMIO_BASE+0x00201028))
#define UART0_LCRH      ((MMIO_BASE+0x0020102C))
#define UART0_CR        ((MMIO_BASE+0x00201030))
#define UART0_IMSC      ((MMIO_BASE+0x00201038))
#define UART0_ICR       ((MMIO_BASE+0x00201044))
void uart_init();
void uart_send(unsigned int c);
char uart_getc();
void uart_puts(char *s);
void uart_hex(unsigned int d);
void uart_send_int(int n);
char uart_recv();
void putc (char *p ,char c);
unsigned int getRegister(unsigned int address);
void setRegister(unsigned int address, unsigned int value);
