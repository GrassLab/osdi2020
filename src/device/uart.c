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

#include "type.h"
#include "device/gpio.h"
#include "device/mbox.h"
#include "task/sysCallTable.h"
#include "task/sysCall.h"
#include "task/taskManager.h"

/* PL011 UART registers */
#define UART0_DR ((volatile uint32_t *)(MMIO_BASE + 0x00201000))
#define UART0_FR ((volatile uint32_t *)(MMIO_BASE + 0x00201018))
#define UART0_IBRD ((volatile uint32_t *)(MMIO_BASE + 0x00201024))
#define UART0_FBRD ((volatile uint32_t *)(MMIO_BASE + 0x00201028))
#define UART0_LCRH ((volatile uint32_t *)(MMIO_BASE + 0x0020102C))
#define UART0_CR ((volatile uint32_t *)(MMIO_BASE + 0x00201030))
#define UART0_IMSC ((volatile uint32_t *)(MMIO_BASE + 0x00201038))
#define UART0_ICR ((volatile uint32_t *)(MMIO_BASE + 0x00201044))

/**
 * Set baud rate and characteristics (115200 8N1) and map to GPIO
 */
void uartInit()
{
    register uint32_t r;

    /* initialize UART */
    *UART0_CR = 0; // turn off UART0

    setUartClock();

    /* map UART0 to GPIO pins */
    r = *GPFSEL1;
    r &= ~((7 << 12) | (7 << 15)); // gpio14, gpio15
    r |= (4 << 12) | (4 << 15);    // alt0
    *GPFSEL1 = r;
    *GPPUD = 0; // enable pins 14 and 15
    r = 150;
    while (r--)
    {
        asm volatile("nop");
    }
    *GPPUDCLK0 = (1 << 14) | (1 << 15);
    r = 150;
    while (r--)
    {
        asm volatile("nop");
    }
    *GPPUDCLK0 = 0; // flush GPIO setup

    *UART0_ICR = 0x7FF; // clear interrupts
    *UART0_IBRD = 2;    // 115200 baud
    *UART0_FBRD = 0xB;
    *UART0_LCRH = 0b11 << 5; // 8n1
    *UART0_CR = 0x301;       // enable Tx, Rx, FIFO
}

/**
 * Send a character
 */
void uartSend(uint32_t c)
{
    /* wait until we can send */
    do
    {
        asm volatile("nop");
    } while (*UART0_FR & 0x20);
    /* write the character to the buffer */
    *UART0_DR = c;
}

/**
 * Receive a character
 */
char uartGetc()
{
    char r;
    /* wait until something is in the buffer */
    do
    {
        asm volatile("nop");
    } while (*UART0_FR & 0x10);
    /* read it and return */
    r = (char)(*UART0_DR);
    /* convert carrige return to newline */
    return r == '\r' ? '\n' : r;
}

void _sysUartWrite()
{
    uint32_t sp_begin = &kstack_pool[current->task_id + 1];
    uint32_t c = *(uint32_t *)(sp_begin - 32 * 8);

    uartSend(c);
}

void _sysUartRead()
{
    uint32_t sp_begin = &kstack_pool[current->task_id + 1];

    char c = uartGetc();
    *(uint32_t *)(sp_begin - 32 * 8) = c;
}

/**
 * Receive a character without converting carrige return
 */
char uartGetcWithCR()
{
    char r;
    /* wait until something is in the buffer */
    do
    {
        asm volatile("nop");
    } while (*UART0_FR & 0x10);
    /* read it and return */
    r = (char)(*UART0_DR);
    /* convert carrige return to newline */
    return r;
}

/**
 * Display a string
 */
void uartPuts(char *s)
{
    while (*s)
    {
        /* convert newline to carrige return + newline */
        if (*s == '\n')
            // uartWrite('\r');
            uartSend('\r');
        // uartWrite(*s++);
        uartSend(*s++);
    }
}

/**
 * Display a binary value in hexadecimal
 */
void uartHex(uint32_t d)
{
    uartPuts("0x");

    uint32_t n;
    int32_t c;
    for (c = 28; c >= 0; c -= 4)
    {
        // get highest tetrad
        n = (d >> c) & 0xF;
        // 0-9 => '0'-'9', 10-15 => 'A'-'F'
        n += n > 9 ? 0x37 : 0x30;
        uartSend(n);
        // uartWrite(n);
    }

    return;
}

void uartInt(uint32_t i)
{
    if (i == 0)
    {
        // uartWrite('0');
        uartSend('0');
        return;
    }

    char buf[256];
    uint32_t buf_ptr = 0;
    while (i > 0)
    {
        buf[buf_ptr++] = (i % 10) + '0';
        i = i / 10;
    }
    buf[buf_ptr] = '\0';
    for (uint32_t e = buf_ptr - 1, s = 0, half = (buf_ptr - 1) / 2; e > half; --e, ++s)
    {
        char tmp = buf[s];
        buf[s] = buf[e];
        buf[e] = tmp;
    }

    uartPuts(buf);
}

void uartFloat(double f)
{
    uint32_t i = (uint32_t)f;
    uint32_t frac = (uint32_t)((f - (double)i) * 100000);
    uartInt(i);
    uartPuts(".");
    uartInt(frac);
}