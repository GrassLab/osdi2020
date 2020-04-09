#include "../include/gpio.h"
#include "../include/mailbox.h"

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
    mbox[0] = 9*4;
    mbox[1] = MBOX_REQUEST;
    mbox[2] = MBOX_TAG_SETCLKRATE; // set clock rate
    mbox[3] = 12;
    mbox[4] = 8;
    mbox[5] = 2;           // UART clock
    mbox[6] = 4000000;     // 4Mhz
    mbox[7] = 0;           // clear turbo
    mbox[8] = MBOX_TAG_LAST;
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
void uart_puts(char *s) 
{
    while(*s) {
        /* convert newline to carrige return + newline */
        if(*s=='\n')
            uart_send('\r');
        uart_send(*s++);
    }
}

/*
 * Transfer int to char
 */
char uart_i2c(unsigned int d) 
{
    return (d<10) ? d+48 :0;
}

/**
 * Display a binary value in hexadecimal
 */
void uart_hex(unsigned int d) 
{
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

/**
 * Compare two string and return 0 if they are identical
 */ 
int uart_strcmp(const char *cs, const char *ct)
{
    unsigned char c1, c2;
    while (1) {
        c1 = *cs++;
        c2 = *ct++;
        if (c1 != c2) return c1 < c2 ? -1 : 1;
        if (!c1) break;
    }
    return 0;
}

/**
 * Copy memory by address and size
 */ 
void *uart_memcpy (const void *src, void *dst, int len)
{
    const char *s = src;
    char *d = dst;
    while (len--) {
        *d++ = *s++;
        //if (*s == 0) break;
    }
    return dst;
}
/**
 * Transfer string to int
 */
/*
unsigned int uart_c2i(char *s) 
{
    return (d<10) ? d+48 :0;
}
*/
int uart_atoi(char *dst, int d) 
{
    int sign, i;
    char tmpstr[19];
    // check input
    sign=0;
    if(d<0) {
        d*=-1;
        sign++;
    }
    if(d>2147483647) {
        d=2147483647;
    }
    // convert to string
    i=18;
    tmpstr[i]=0;
    do {
        tmpstr[--i]='0'+(d%10);
        d/=10;
    } while(d!=0 && i>0);
    if(sign) {
        tmpstr[--i]='-';
    }
    uart_memcpy(&tmpstr[i], dst, 18-i);
    dst[18-i] = 0;
    return 18-i;
}