#include "string.h"
#include "uart.h"
#include "mailbox.h"
#include "utility.h"

char uart_buffer[2048];
int uart_buffer_front = 0;
int uart_buffer_end = 0;

/**
 * Set baud rate and characteristics (115200 8N1) and map to GPIO
 */
void uart_init()
{
    register unsigned int r;

    /* initialize UART */
    setRegister(UART0_CR, 0);         // turn off UART0

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

    setRegister(UART0_ICR, 0x7FF);
    setRegister(UART0_IBRD, 2);
    setRegister(UART0_FBRD, 0xB);
    setRegister(UART0_LCRH, 0b11<<5);
    setRegister(UART0_CR, 0x301);
    // setRegister(UART0_IMSC, 3 << 4);
    // setRegister(IRQ_ENABLE_REGISTER_2, 1 << 25);
    memset(uart_buffer, 0 , sizeof(uart_buffer));
}

/**
 * Send a character
 */
void uart_send(unsigned int c) {
    /* wait until we can send */
    do{asm volatile("nop");} while(getRegister(UART0_FR)&0x20);
    /* write the character to the buffer */
    setRegister(UART0_DR, c);
}

/**
 * Receive a raw character
 */
char uart_recv_char() {
    /* wait until something is in the buffer */
    do{asm volatile("nop");} while(getRegister(UART0_FR)&0x10);
    /* read it and return */
    return (char)(getRegister(UART0_DR));
}

/**
 * Receive a character, and replace carriage return
 */
char uart_getc() {
    char r;
    /* wait until something is in the buffer */
    do{asm volatile("nop");} while(getRegister(UART0_FR)&0x10);
    /* read it and return */
    r = (char)(getRegister(UART0_DR));
    /* convert carrige return to newline */
    return r == '\r' ? '\n' : r;
    if (!uart_buffer_empty()) {
        char c = uart_buffer[uart_buffer_end];
        uart_buffer_end = (uart_buffer_end+1) % 2048;
        return c;
    }
}

int uart_buffer_empty() {
    return uart_buffer_front == uart_buffer_end;
}

void uart_save(char c) {
    uart_buffer[uart_buffer_front] = (c == '\r') ? '\n' : c;
    uart_buffer_front = (uart_buffer_front+1) % 2048;
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