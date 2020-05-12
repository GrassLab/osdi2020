#include "uart.h"

#include "gpio.h"
#include "mailbox.h"
#include "peri_base.h"

/**
 * Set baud rate and characteristics (115200 8N1) and map to GPIO
 */
void uart_init()
{
    register unsigned int r;

    /* initialize UART */
    UART_REG-> CR = 0;                                  // turn off UART0

    /* set up clock for consistent divisor values */
    mbox[0] = 9*4;                                      // buffer size in bytes
    mbox[1] = MBOX_REQUEST_CODE;                        // buffer request/response code
    mbox[2] = MBOX_TAG_SET_CLK_RATE;                    // tag identifier
    mbox[3] = 3*4;                                      // value buffer size in bytes
    mbox[4] = MBOX_TAG_REQUEST_CODE;                    // tag request/response code
    mbox[5] = MBOX_CLK_ID_UART;                         // clock id
    mbox[6] = 4000000;                                  // rate (in Hz)
    mbox[7] = 0;                                        // setting turbo
    mbox[8] = MBOX_END_TAG;
    mbox_call(MBOX_CH_PROPT_ARM_VC);

    /* map UART0 to GPIO pins */
    r=*GPFSEL1;
    r&=~((7<<12)|(7<<15));                              // gpio14, gpio15
    r|=(4<<12)|(4<<15);                                 // alt0, UART0
    *GPFSEL1 = r;
    *GPPUD = 0;                                         // enable pins 14 and 15
    r=150; while(r--) { asm volatile("nop"); }
    *GPPUDCLK0 = (1<<14)|(1<<15);
    r=150; while(r--) { asm volatile("nop"); }
    *GPPUDCLK0 = 0;                                     // flush GPIO setup

    UART_REG-> IBRD = 2;                                // 115200 baud
    UART_REG-> FBRD = 0xB;  
    UART_REG-> LCRH |= (UART_LCRH_WLEN_8 | UART_LCRH_FEN);
    UART_REG-> CR |= (UART_CR_TXE | UART_CR_RXE);
    UART_REG-> CR |= UART_CR_UARTEN;
    UART_REG-> ICR = 0x7F2;
}

/**
 * Send a character by uart
 */
void uart_send(char c) {
    do{/* wait until we can send */
        asm volatile("nop");
    }while(UART_REG-> FR & UART_FR_TXFF);
    /* write the character to the buffer */
    UART_REG-> DR = c;
}

/**
 * Receivce character by uart
 */
char uart_recv() {
    char c;
    do{/* wait until something is in the buffer */
        asm volatile("nop");
    }while(UART_REG-> FR & UART_FR_RXFE);
    /* read it and return */
    c = (char)(UART_REG-> DR);
    if (UART_REG-> RSRECR)
        uart_puts("WARNING! some error occured when UART was receiving data!");
    return c;
}

/**
 * UART user interface receive character
 */
char uart_getc() {
    char r = uart_recv();
    /* convert carrige return to newline */
    return r == '\r' ? '\n' : r;
}

/**
 * UART user interface put character
 */
void uart_putc(void* p, char c) {
    /* convert newline to carrige return + newline */
    if(c == '\n')
        uart_send('\r');
    uart_send(c);
}

/**
 * UART user interface put string
 */
void uart_puts(char *s) {
    while(*s) {
        uart_putc(0, *s++);
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