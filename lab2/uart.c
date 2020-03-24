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

#include "gpio.h"
#include "mbox.h"
#include "string.h"
#include <stdarg.h>

/* PL011 UART registers */
#define UART0_DR        ((volatile unsigned int*)(MMIO_BASE+0x00201000))
#define UART0_FR        ((volatile unsigned int*)(MMIO_BASE+0x00201018))
#define UART0_IBRD      ((volatile unsigned int*)(MMIO_BASE+0x00201024))
#define UART0_FBRD      ((volatile unsigned int*)(MMIO_BASE+0x00201028))
#define UART0_LCRH      ((volatile unsigned int*)(MMIO_BASE+0x0020102C))
#define UART0_CR        ((volatile unsigned int*)(MMIO_BASE+0x00201030))
#define UART0_IMSC      ((volatile unsigned int*)(MMIO_BASE+0x00201038))
#define UART0_ICR       ((volatile unsigned int*)(MMIO_BASE+0x00201044))


/**
 * Set baud rate and characteristics (115200 8N1) and map to GPIO
 */
void uart_init()
{
    register unsigned int r;

    /* turn off uart0 */
    *UART0_CR = 0;

    /* setup clock for consistent divisor values */
    mbox[0] = 9*4;
    mbox[1] = MBOX_REQUEST;
    /* ====== /Tags begin ====== */
    mbox[2] = MBOX_TAG_SETCLKRATE; // set clock rate
    /* buffer size */
    mbox[3] = 12;
    mbox[4] = 8;
    mbox[5] = 2;           // UART clock
    mbox[6] = 4000000;     // 4Mhz
    mbox[7] = 0;           // clear turbo
    mbox[8] = MBOX_TAG_LAST;
    /* ====== Tags end/ ======== */

    mbox_call(MBOX_CH_PROP);

    /* map uart0 to GPIO pins */
    r =  *GPFSEL1;
    r &= ~((7<<12)|(7<<15)); // gpio14, gpio15
    r |= (4<<12)|(4<<15);    // alt0
    *GPFSEL1 = r;
    *GPPUD = 0;            // enable pins 14 and 15
    r = 150; while (r--) { asm volatile("nop"); }
    *GPPUDCLK0 = (1 << 14) | (1 << 15);
    r = 150; while (r--) { asm volatile("nop"); }
    *GPPUDCLK0 = 0; // flush GPIO setup

    *UART0_ICR = 0x7FF; // clear interrupts
    *UART0_IBRD = 2;    // 115200 baud
    *UART0_FBRD = 0xB;
    *UART0_LCRH =
        0b11 << 5; // configure the WLEN part: 0x3 for representing 8bits
    /* 8n1: one start bit, eight (8) data bits, no (N) parity bit, and one (1)
     * stop bit */
    *UART0_CR = 0x301; // enable Tx, Rx, FIFO */
}


/**
 * Send a character
 */
void uart_send(unsigned int c) {
    /* wait until we can send */
    do{asm volatile("nop");}while(*UART0_FR&0x20);
    /* write the character to the buffer */
    *UART0_DR=c;
}

/**
 * Flush the uart value
 */
void uart_flush() {
  /* The bit 0 is show that if the receive FIFO holds at least 1 symbol */
  /* so this function will eat all the data inside the FIFO */
  /* until there are not exist any symbol */
  while (*UART0_FR&0x01) {
    (*UART0_DR);
  }
}

/**
 * Receive a character
 */
char uart_getc() {
    char r;
    /* wait until something is in the buffer */
    do{asm volatile("nop");}while(*UART0_FR&0x10);
    /* read it and return */
    return (char)(*UART0_DR);
    /* convert carrige return to newline */
}

/**
 * Display a string
 */
void uart_puts(char *s) {
    while(*s) {
      uart_send(*s++);
    }
}

/**
 * Display a binary value in hexadecimal
 */
void uart_hex(unsigned int d) {
    unsigned int n;
    int c;
    for(c=28;c>=0;c-=4) {
        // get highest tetrad
        n=(d>>c)&0xF;
        // 0-9 => '0'-'9', 10-15 => 'A'-'F'
        n+=n>9?0x37:0x30;
        uart_send(n);
    }
}

void uart_println(char *format, ...) {
  unsigned int i;
  unsigned int n;
  char *s;

  va_list arg;
  va_start(arg, format);

  for (char *traverse = format; *traverse != 0; traverse++) {
    while (*traverse != '%' && *traverse != 0) {
      uart_send(*traverse++);
    }

    if (*traverse == 0) break;

    /* move to the hole */
    traverse++;
    /* actions */
    switch (*traverse) {
    case 'c':
      i = va_arg(arg, int);
      uart_send(i);
      break;
    case 'd':
      i = va_arg(arg, int);
      if (i < 0) {
        i = -i;
        uart_send('-');
      }
      uart_puts(itoa(i, 10));
      break;
    case 'x':
      i = va_arg(arg, unsigned int);
      for (int c = 28; c >= 0; c-= 4) {
        /* get highest tetrad */
        n = (i >> c) & 0xF;
        /* 0-9 => '0'-'9', 10-15 => 'A'-'F' */
        n += (n > 9)? 0x37:0x30;
        uart_send(n);
      }
      break;
    case 's':
      s = va_arg(arg, char*);
      uart_puts(s);
      break;
    default:
      uart_send('%');
      uart_send(*traverse);
      break;
    }
  }

  uart_puts("\r\n");

  va_end(arg);
}
