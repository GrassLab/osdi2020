#include "string.h"
#include "uart.h"
#include "mailbox.h"

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
    mbox[1] = TAG_REQUEST_CODE;
    mbox[2] = MBOX_TAG_SETCLKRATE; // set clock rate
    mbox[3] = 12;
    mbox[4] = 8;
    mbox[5] = 2;           // UART clock
    mbox[6] = 4000000;     // 4Mhz
    mbox[7] = 0;           // clear turbo
    mbox[8] = TAG_END;
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
    do{asm volatile("nop");} while(*UART0_FR&0x20);
    /* write the character to the buffer */
    *UART0_DR = c;
}

/**
 * Receive a raw character
 */
char uart_recv_char() {
    /* wait until something is in the buffer */
    do{asm volatile("nop");} while(*UART0_FR&0x10);
    /* read it and return */
    return (char)(*UART0_DR);
}

/**
 * Receive a character, and replace carriage return
 */
char uart_getc() {
    char r;
    /* wait until something is in the buffer */
    do{asm volatile("nop");} while(*UART0_FR&0x10);
    /* read it and return */
    r = (char)(*UART0_DR);
    /* convert carrige return to newline */
    return r == '\r' ? '\n' : r;
}

void uart_get_string(char *str) {
    int index = 0;
    char c = uart_getc();
    while(c != '\n') {
        str[index++] = c;
        uart_send(c);
        c = uart_getc();
    }
    str[index] = '\0';
}

/**
 * Display a string
 */
void uart_puts(char *s) {
    while(*s) {
        /* convert newline to carrige return + newline */
        if(*s=='\n') uart_send('\r');
        uart_send(*s++);
    }
}

void uart_print_int(int num) {
    char str[1024] = {0};
    intToStr(num, str);
    uart_puts(str);
}

void uart_print_double(double num) {
    char str[1024] = {0};
    doubleToStr(num, str);
    uart_puts(str);
}

void uart_print_hex(unsigned int num) {
    char str[1024] = {0};
    hexToStr(num, str);
    uart_puts(str);
}

void uart_print_hex_array(unsigned int *num, int size) {
    char str[1024] = {0};
    for(int i = 0; i < size; i++) {
        hexToStrNoTruncat(num-i, str+(8*i));
    }
    uart_puts(str);
}

void print_uart_clock()
{
    mbox[0] = 9*4;
    mbox[1] = TAG_REQUEST_CODE;
    mbox[2] = MBOX_TAG_GETCLKRATE;
    mbox[3] = 8;
    mbox[4] = TAG_REQUEST_CODE;
    mbox[5] = 0x2;
    mbox[6] = 0;            
    mbox[7] = 0;           
    mbox[8] = TAG_END;
    if (mbox_call(MBOX_CH_PROP)) {
        uart_puts("uart clock: 0x");
        uart_print_hex(mbox[6]);
        uart_puts("\n");
    } else uart_puts("Unable to read uart clock!");
}