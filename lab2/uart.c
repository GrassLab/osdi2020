#include "uart.h"
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

    set_uart0_clock_rate();

    if(mbox_call(MBOX_CH_PROP)){

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
    }else{
        uart_puts("Unable to query serial!\n");
    }
}

/**
 * Send a character
 */
void uart_send(unsigned int c) {
    /* wait until we can send */
    //check 6th 100000 bit if it can send
    //if Receive FIFO full, nop
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
    //check 5th 10000 bit if it can send 
    //if Transmit FIFO full, nop
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
        /* convert newline to carrige return + newline*/ 
        //In qemu we need to chenge \n in to \r
        if(*s=='\n')
            uart_send('\r');
        uart_send(*s++);
    }
}
/**
 * Display a binary value in hexadecimal
 */
void uart_hex(unsigned int d) {
    unsigned int n;
    int c;
    uart_puts("0x");
    for(c=28;c>=0;c-=4) {
        // get highest tetrad
        n=(d>>c)&0xF;
        // 0-9 => '0'-'9', 10-15 => 'A'-'F'
        n+=n>9?0x37:0x30;
        uart_send(n);
    }
}