/*
 * uart.c
 * Uart stdio
 */

#include "mm.h"
#include "uart.h"
#include "string.h"
#include "mailbox.h"

#define UART0_DR        ((volatile unsigned int*)(MMIO_BASE+0x00201000))
#define UART0_FR        ((volatile unsigned int*)(MMIO_BASE+0x00201018))
#define UART0_IBRD      ((volatile unsigned int*)(MMIO_BASE+0x00201024))
#define UART0_FBRD      ((volatile unsigned int*)(MMIO_BASE+0x00201028))
#define UART0_LCRH      ((volatile unsigned int*)(MMIO_BASE+0x0020102C))
#define UART0_CR        ((volatile unsigned int*)(MMIO_BASE+0x00201030))
#define UART0_IMSC      ((volatile unsigned int*)(MMIO_BASE+0x00201038))
#define UART0_ICR       ((volatile unsigned int*)(MMIO_BASE+0x00201044))


// https://wiki.osdev.org/Raspberry_Pi_Bare_Bones
void uart_setup()
{
    mm_write(UART0_CR, 0);

    mm_write(GPPUD,0);
    delay(150);
    mm_write(GPPUDCLK0,(1<<14)|(1<<15));
    delay(150);
    mm_write(GPPUDCLK0,0);

    mm_write(UART0_ICR, 0x7FF);    // clear interrupts

    mbox[0] = 9*4;
    mbox[1] = REQUEST_CODE;
    mbox[2] = SET_CLK_RATE; // set clock rate
    mbox[3] = 12;
    mbox[4] = 8;
    mbox[5] = 2;           // UART clock
    mbox[6] = 3000000;     // 4Mhz
    mbox[7] = 0;           // skip turbo
    mbox[8] = END_TAG;
    mbox_call(MBOX_CH_PROP);

    // Divider = 3000000 / (16 * 115200) = 1.627 = ~1.
	mm_write(UART0_IBRD, 1);
	// Fractional part register = (.627 * 64) + 0.5 = 40.6 = ~40.
	mm_write(UART0_FBRD, 40);
 
	// Enable FIFO & 8 bit data transmission (1 stop bit, no parity).
	mm_write(UART0_LCRH, (1 << 4) | (1 << 5) | (1 << 6));
 
	// Mask all interrupts.
	mm_write(UART0_IMSC, (1 << 1) | (1 << 4) | (1 << 5) | (1 << 6) |
	                       (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10));
 
	// Enable UART0, receive & transfer part of UART.
	mm_write(UART0_CR, (1 << 0) | (1 << 8) | (1 << 9)); 

}

void uart_putc(uint8_t c)
{
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

void uart_hex(unsigned int d) {
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

void uart_read_line(char *buffer, size_t size) {
    size_t position = 0;
    uint8_t c, e;

    while(position < size) {
        c = uart_getc();

        if (c == '\r' || c == '\n') {
            buffer[position++] = '\0';
            uart_puts("\r\n");
            return;
        } else if (c == 127) {
            if (position > 0) {
                buffer[--position] = 0;
                uart_puts("\b \b");
            }
        } else if (c == '[') {
            e = uart_getc();
            if (e == 'C' && position < strlen(buffer)) {
                uart_puts("\033[C");
                position++;
            } else if (e == 'D' && position > 0) {
                uart_puts("\033[D");
                position--;
            }
        } else if (c > 39 && c < 127) {
            buffer[position++] = c;
            uart_putc(c);
        }
    }
}
