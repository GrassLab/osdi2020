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


/* PL011 UART registers */
#define UART0_DR        ((volatile unsigned int*)(MMIO_BASE+0x00201000))
#define UART0_FR        ((volatile unsigned int*)(MMIO_BASE+0x00201018))
#define UART0_IBRD      ((volatile unsigned int*)(MMIO_BASE+0x00201024))
#define UART0_FBRD      ((volatile unsigned int*)(MMIO_BASE+0x00201028))
#define UART0_LCRH      ((volatile unsigned int*)(MMIO_BASE+0x0020102C))
#define UART0_CR        ((volatile unsigned int*)(MMIO_BASE+0x00201030))
#define UART0_IMSC      ((volatile unsigned int*)(MMIO_BASE+0x00201038))
#define UART0_ICR       ((volatile unsigned int*)(MMIO_BASE+0x00201044))

/* Auxilary mini UART registers */
#define AUX_ENABLE      ((volatile unsigned int*)(MMIO_BASE+0x00215004))
#define AUX_MU_IO       ((volatile unsigned int*)(MMIO_BASE+0x00215040))
#define AUX_MU_IER      ((volatile unsigned int*)(MMIO_BASE+0x00215044))
#define AUX_MU_IIR      ((volatile unsigned int*)(MMIO_BASE+0x00215048))
#define AUX_MU_LCR      ((volatile unsigned int*)(MMIO_BASE+0x0021504C))
#define AUX_MU_MCR      ((volatile unsigned int*)(MMIO_BASE+0x00215050))
#define AUX_MU_LSR      ((volatile unsigned int*)(MMIO_BASE+0x00215054))
#define AUX_MU_MSR      ((volatile unsigned int*)(MMIO_BASE+0x00215058))
#define AUX_MU_SCRATCH  ((volatile unsigned int*)(MMIO_BASE+0x0021505C))
#define AUX_MU_CNTL     ((volatile unsigned int*)(MMIO_BASE+0x00215060))
#define AUX_MU_STAT     ((volatile unsigned int*)(MMIO_BASE+0x00215064))
#define AUX_MU_BAUD     ((volatile unsigned int*)(MMIO_BASE+0x00215068))

/**
 * mode 0 = PL011, mode 1 = mini UART
 */
static int uart_mode = 0;

/**
 * Set baud rate and characteristics (115200 8N1) and map to GPIO
 */
void uart1_init()
{	
    register unsigned int r;
	uart_mode = 1;

	/* initialize UART */
	*AUX_ENABLE |=1;       // enable UART1, AUX mini uart
	*AUX_MU_CNTL = 0;
	*AUX_MU_LCR = 3;       // 8 bits
	*AUX_MU_MCR = 0;
	*AUX_MU_IER = 0;
	*AUX_MU_IIR = 0xc6;    // disable interrupts
	*AUX_MU_BAUD = 270;    // 115200 baud
	/* map UART1 to GPIO pins */
	r=*GPFSEL1;
	r&=~((7<<12)|(7<<15)); // gpio14, gpio15
	r|=(2<<12)|(2<<15);    // alt5
	*GPFSEL1 = r;
	*GPPUD = 0;            // enable pins 14 and 15
	r=150; while(r--) { asm volatile("nop"); }
	*GPPUDCLK0 = (1<<14)|(1<<15);
	r=150; while(r--) { asm volatile("nop"); }
	*GPPUDCLK0 = 0;        // flush GPIO setup
	*AUX_MU_CNTL = 3;      // enable Tx, Rx
}

void uart0_init()
{
    register unsigned int r;
	uart_mode = 0;

	/* initialize UART */
	*UART0_CR = 0;         // turn off UART0

	/* set up clock for consistent divisor values */
	mbox[0] = 9*4;
	mbox[1] = MBOX_REQUEST;
	mbox[2] = MBOX_TAG_SETCLKRATE; // set clock rate
	mbox[3] = 12;
	mbox[4] = 8;
	mbox[5] = 2;           // UART clock
	mbox[6] = 4000000;     // 4Mhz
	mbox[7] = 0;           // clear turbo
	mbox[8] = MBOX_TAG_LAST;
	//mbox_call(MBOX_CH_PROP);

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
	*UART0_IBRD = 2;       // 115200 baud
	*UART0_FBRD = 0xB;
	*UART0_LCRH = 0b11<<5; // 8n1
	*UART0_CR = 0x301;     // enable Tx, Rx, FIFO
}

/**
 * Send a character
 */
void uart_send(unsigned int c) {
	if(uart_mode) {
		/* wait until we can send */
		do{asm volatile("nop");}while(!(*AUX_MU_LSR&0x20));
		/* write the character to the buffer */
		*AUX_MU_IO=c;
	}
	else {
	    /* wait until we can send */
	    do{asm volatile("nop");}while(*UART0_FR&0x20);
		/* write the character to the buffer */
		*UART0_DR=c;
	}
}

/**
 * Receive a character
 */
char uart_getc() {
    char r;
	if(uart_mode) {
		/* wait until something is in the buffer */
		do{asm volatile("nop");}while(!(*AUX_MU_LSR&0x01));
		/* read it and return */
		r=(char)(*AUX_MU_IO);
	}
	else {
		/* wait until something is in the buffer */
		do{asm volatile("nop");}while(*UART0_FR&0x10);
		/* read it and return */
		r=(char)(*UART0_DR);
	}
    /* convert carrige return to newline */
    return r=='\r'?'\n':r;
}

/**
 * Display a string
 */
void uart_puts(char *s) {
    while(*s) {
        /* convert newline to carrige return + newline */
        if(*s=='\n')
            uart_send('\r');
        uart_send(*s++);
    }
}

int mode() {
	return uart_mode;
}
