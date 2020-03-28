#include "gpio.h"
#include "common.h"
#include "string.h"
#include "gpio.h"
#include "mailbox.h"

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

    /* initialize UART */
    *UART0_CR = 0;         // turn off UART0

    /* set up clock for consistent divisor values */
    mailbox[0] = 9*4;
    mailbox[1] = MBOX_REQUEST;
    mailbox[2] = MBOX_TAG_SETCLKRATE; // set clock rate
    mailbox[3] = 12;
    mailbox[4] = 8;
    mailbox[5] = 2;           // UART clock
    mailbox[6] = 4000000;     // 4Mhz
    mailbox[7] = 0;           // clear turbo
    mailbox[8] = MBOX_TAG_LAST;
    mailbox_call(MBOX_CH_PROP);

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
    /* wait until we can send */
    do{asm volatile("nop");}while(*UART0_FR&0x20);
    /* write the character to the buffer */
    *UART0_DR=c;
}

/**
 * Receive a character
 */
char uart_getc() {
    char r;
    /* wait until something is in the buffer */
    do{asm volatile("nop");}while(*UART0_FR&0x10);
    /* read it and return */
    r=(char)(*UART0_DR);
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

void uart_send_int(int n)
{
    char s[1024];
    itoa(n, s, 10);

    uart_puts(s);
}

void uart_send_hex(int n)
{
    char s[1024];
    itoa(n, s, 16);

    uart_puts("0x");
    uart_puts(s);
}

void uart_send_float(float f, int n)
{
    char s[1024];
    ftoa(f, s, n);
    uart_puts(s);
}

int uart_print(char *s)
{
    uart_puts(s);
}

int uart_gets(char *buf, int buf_size)
{
    int i = 0;
    char c;

    do
    {
        c = uart_getc();

        c = c == '\r' ? '\n' : c;

        if (c == 8 || c == 127)
        {
            if (i > 0)
            {
                buf[i--] = '\0';
                uart_send(8);
                uart_send(' ');
                uart_send(8);
            }
        }
        else if(c != '\n')
        {
            buf[i++] = c;
            // ensure users can see what they type
            uart_send(c);
        }
    } while (c != '\n' && i < buf_size - 1);

    // replace '\n' with NULL
    if(i > 0)
        buf[i] == '\0';

    if (i == buf_size)
        return -1;

    return i;
}
