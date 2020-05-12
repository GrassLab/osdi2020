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

#ifndef UART_H
#define UART_H

#ifdef MINIUART

/* mini UART registers */
#define UART_TYPE "MINI_UART"

#define AUX_IRQ ((volatile unsigned int *)(MMIO_BASE + 0x00215000))
#define AUX_ENABLES ((volatile unsigned int *)(MMIO_BASE + 0x00215004))
#define AUX_MU_CNTL_REG ((volatile unsigned int *)(MMIO_BASE + 0x00215060))
#define AUX_MU_IER_REG ((volatile unsigned int *)(MMIO_BASE + 0x00215044))
#define AUX_MU_LCR_REG ((volatile unsigned int *)(MMIO_BASE + 0x0021504C))
#define AUX_MU_MCR_REG ((volatile unsigned int *)(MMIO_BASE + 0x00215050))
#define AUX_MU_BAUD_REG ((volatile unsigned int *)(MMIO_BASE + 0x00215068))
#define AUX_MU_IIR_REG ((volatile unsigned int *)(MMIO_BASE + 0x00215048))
#define AUX_MU_CNTL_REG ((volatile unsigned int *)(MMIO_BASE + 0x00215060))
#define AUX_MU_IO_REG ((volatile unsigned int *)(MMIO_BASE + 0x00215040))
#define AUX_MU_LSR_REG ((volatile unsigned int *)(MMIO_BASE + 0x00215054))
#define AUX_MU_MSR_REG ((volatile unsigned int *)(MMIO_BASE + 0x00215058))
#define AUX_MU_SCRATCH ((volatile unsigned int *)(MMIO_BASE + 0x0021505C))
#define AUX_MU_CNTL_REG ((volatile unsigned int *)(MMIO_BASE + 0x00215060))
#define AUX_MU_STAT_REG ((volatile unsigned int *)(MMIO_BASE + 0x00215064))
#define AUX_MU_BAUD_REG ((volatile unsigned int *)(MMIO_BASE + 0x00215068))

#define IER_REG_EN_REC_INT (1 << 0)
#define IER_REG_INT (3 << 2) // Must be set to receive interrupts
#define IER_REG_VALUE (IER_REG_EN_REC_INT | IER_REG_INT)
#define IIR_REG_REC_NON_EMPTY (2 << 1)

#else

#define UART_TYPE "PL011_UART"
/* PL011 UART registers */

#define UART0_DR ((volatile unsigned int *)(MMIO_BASE + 0x00201000))
#define UART0_FR ((volatile unsigned int *)(MMIO_BASE + 0x00201018))
#define UART0_IBRD ((volatile unsigned int *)(MMIO_BASE + 0x00201024))
#define UART0_FBRD ((volatile unsigned int *)(MMIO_BASE + 0x00201028))
#define UART0_LCRH ((volatile unsigned int *)(MMIO_BASE + 0x0020102C))
#define UART0_CR ((volatile unsigned int *)(MMIO_BASE + 0x00201030))
#define UART0_IMSC ((volatile unsigned int *)(MMIO_BASE + 0x00201038))
#define UART0_ICR ((volatile unsigned int *)(MMIO_BASE + 0x00201044))

#endif

void uart_init();
void uart_send(unsigned int c);
char uart_recv();
void uart_puts(char *s);
void uart_flush(void);

#endif
