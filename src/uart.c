/*
 * uart.c
 * Uart stdio
 */

#include "mm.h"
#include "uart.h"
#include "string.h"
#include "mailbox.h"
#include "time.h"

#define UART0_DR        ((volatile unsigned int*)(MMIO_BASE+0x00201000))
#define UART0_FR        ((volatile unsigned int*)(MMIO_BASE+0x00201018))
#define UART0_IBRD      ((volatile unsigned int*)(MMIO_BASE+0x00201024))
#define UART0_FBRD      ((volatile unsigned int*)(MMIO_BASE+0x00201028))
#define UART0_LCRH      ((volatile unsigned int*)(MMIO_BASE+0x0020102C))
#define UART0_CR        ((volatile unsigned int*)(MMIO_BASE+0x00201030))
#define UART0_IMSC      ((volatile unsigned int*)(MMIO_BASE+0x00201038))
#define UART0_ICR       ((volatile unsigned int*)(MMIO_BASE+0x00201044))

/**
 * https://wiki.osdev.org/Raspberry_Pi_Bare_Bones
 */
void uart_setup()
{
    unsigned int r;

    mm_write(UART0_CR, 0);

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

    mm_write(UART0_ICR, 0x7FF);    // clear interrupts
   
    volatile unsigned int  __attribute__((aligned(16))) mbox[9] = {
        9*4, REQUEST_CODE, SET_CLK_RATE, 12, 8, 2, 4000000, 0 ,END_TAG
    };

    mbox_call(mbox, MBOX_CH_PROP);

    // Divider = 4000000 / (16 * 115200) = 2.170 = ~2.
	mm_write(UART0_IBRD, 2);
	// Fractional part register = (.170 * 64) + 0.5 = 11.38 = ~11.
	mm_write(UART0_FBRD, 11);
 
	// Enable FIFO & 8 bit data transmission (1 stop bit, no parity).
	mm_write(UART0_LCRH, (1 << 4) | (1 << 5) | (1 << 6));
 
	// Mask all interrupts.
	mm_write(UART0_IMSC, (1 << 1) | (1 << 4) | (1 << 5) | (1 << 6) |
	                       (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10));
 
	// Enable UART0, receive & transfer part of UART.
	mm_write(UART0_CR, (1 << 0) | (1 << 8) | (1 << 9)); 
    
    uart_log(LOG_INFO, "UART initialized");
}

void uart_putc(const uint8_t c) {
    while(mm_read(UART0_FR)& (1 << 5));
    mm_write(UART0_DR, c);
}

uint8_t uart_getc() {
    while(mm_read(UART0_FR)& (1 << 4));
    return(mm_read(UART0_DR)&0xFF);
}

void uart_puts(const char *str) {
    for (size_t i = 0; str[i] != '\0'; i++)
        uart_putc((uint8_t)str[i]);
}

void uart_hex(const unsigned int d) {
    unsigned int n;
    int c;
    for(c=28;c>=0;c-=4) {
        // get highest tetrad
        n=(d>>c)&0xF;
        // 0-9 => '0'-'9', 10-15 => 'A'-'F'
        n+=n>9?0x37:0x30;
        uart_putc(n);
    }
}

void uart_log(log_level_t level, const char *message) {
    char time_buffer[11];
    get_timestamp(time_buffer);
    uart_putc('[');
    uart_puts(time_buffer);
    uart_putc(']');
    uart_putc('[');
    switch (level) {
        case LOG_INFO:
            uart_puts("INFO");
            break;
        case LOG_WARNING:
            uart_puts("WARNING");
            break;
        case LOG_ERROR:
            uart_puts("ERROR");
            break;
        default:
            uart_puts("WTF");
    }
    uart_putc(']');
    uart_puts(message);
    uart_puts("\r\n");
}
