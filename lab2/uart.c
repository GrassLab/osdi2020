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

// Loop <delay> times in a way that the compiler won't optimize away
static inline void delay(int count)
{
    asm volatile("__delay_%=: subs %[count], %[count], #1; bne __delay_%=\n"
         : "=r"(count): [count]"0"(count) : "cc");
}


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
void uart_init(unsigned int clock_rate, unsigned int baud_rate)
{
    register unsigned int r;

    unsigned int divider = (unsigned int)(clock_rate / (16 * baud_rate));
    unsigned int temp = (unsigned int)(((clock_rate % (16 * baud_rate)) * 8) / baud_rate);
    unsigned int fraction = (temp >> 1) + (temp & 1);

    /* initialize UART */
    *UART0_CR = 0;         // turn off UART0

    /* set up clock for consistent divisor values */
    mbox[0] = 9*4;
    mbox[1] = MBOX_REQUEST;
    /* ====== /Tags begin ====== */
    mbox[2] = MBOX_TAG_SETCLKRATE; // set clock rate
    mbox[3] = 12;
    mbox[4] = 8;
    mbox[5] = 2;           // UART clock
    mbox[6] = clock_rate;  // 4Mhz
    /* mbox[6] = clock_rate;  // 4Mhz */
    mbox[7] = 0;           // clear turbo
    mbox[8] = MBOX_TAG_LAST;
    /* ====== Tags end/ ======== */

    mbox_call(MBOX_CH_PROP);

    /* map UART0 to GPIO pins */
    r=*GPFSEL1;
    r&=~((7<<12)|(7<<15)); // gpio14, gpio15
    r|=(4<<12)|(4<<15);    // alt0
    *GPFSEL1 = r;
    *GPPUD = 0;            // enable pins 14 and 15
    r=150; while(r--) { asm volatile("nop"); }
    *GPPUDCLK0 = (1<<14)|(1<<15);
    r=150; while(r--) { asm volatile("nop"); }
    *GPPUDCLK0 = 0;        // flush GPIO setup

    *UART0_ICR = 0x7FF;    // clear interrupts
    /* *UART0_IBRD = 2; */
    /* *UART0_FBRD = 0xB; */
    *UART0_IBRD = divider; // 115200 baud
    *UART0_FBRD = fraction;
    *UART0_LCRH = 0b11<<5; // 8n1
    *UART0_CR = 0x301;     // enable Tx, Rx, FIFO
}

/* get the clock rate */
unsigned int uart_getrate() {
    register unsigned int r;

    mbox[0] = 8*4;
    mbox[1] = MBOX_REQUEST;
    /* ====== /Tags begin ====== */
    mbox[2] = MBOX_TAG_GETCLKRATE; /* get clock rate */
    mbox[3] = 8;                  /* buffer size */
    mbox[4] = 0;
    mbox[5] = 2;
    mbox[6] = 0;
    mbox[7] = MBOX_TAG_LAST;
    /* ====== Tags end/ ======== */

    mbox_call(MBOX_CH_PROP);

    return mbox[6];
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
    /* wait until something is in the buffer */
    do{asm volatile("nop");}while(*UART0_FR&0x10);
    /* read it and return */
    return (char)(*UART0_DR);
    /* convert carrige return to newline */
}

int is_digit(char c){
    return c >= '0' && c <= '9';
}

unsigned long uart_getul() {
  unsigned long r = 0, c;
  while (is_digit(c = uart_getc())) {
    r = r * 10 + c - '0';
  }
  return r;
}


int uart_getint() {
  int r = 0, c;
  while (is_digit(c = uart_getc())) {
    r = r * 10 + c - '0';
  }
  return r;
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
