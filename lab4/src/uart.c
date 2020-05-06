#include "gpio.h"
#include "mbox.h"
#include "utils.h"
#include "tools.h"

/* PL011 UART registers */
#define UART0_DR        ((volatile unsigned int*)(MMIO_BASE+0x00201000))
#define UART0_FR        ((volatile unsigned int*)(MMIO_BASE+0x00201018))
#define UART0_IBRD      ((volatile unsigned int*)(MMIO_BASE+0x00201024))
#define UART0_FBRD      ((volatile unsigned int*)(MMIO_BASE+0x00201028))
#define UART0_LCRH      ((volatile unsigned int*)(MMIO_BASE+0x0020102C))
#define UART0_CR        ((volatile unsigned int*)(MMIO_BASE+0x00201030))
#define UART0_IMSC      ((volatile unsigned int*)(MMIO_BASE+0x00201038))
#define UART0_ICR       ((volatile unsigned int*)(MMIO_BASE+0x00201044))

#define UART_FR			0x3F201018
#define UART_DR			0x3F201000

void uart_init();
void uart_send(unsigned int c);
char uart_getc();
int uart_get_string(char *s);
int uart_get_string_with_echo(char *s);
void uart_puts(char *s);
void uart_hex(unsigned int d);
void putc(void *p, char c);

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
/*
	do{asm volatile("nop");}while(*UART0_FR&0x20);
	*UART0_DR=c;
*/
	while(get32((unsigned long)UART0_FR) & (1 << 5));
	put32((unsigned long)UART_DR, c);
}


/**
 * Receive a character
 */
char uart_getc() {
	while(get32((unsigned long)UART0_FR) & (1 << 4));
	return get32((unsigned long)UART0_DR)&0xFF;
/*
	char r;
	do{asm volatile("nop");}while(*UART0_FR&0x10);
	r = (char)(*UART0_DR);
	return r=='\r'?'\n':r;
*/
}

/**
 * Receive a string
 */
int uart_get_string(char * s){
	char r;
	int iter = 0;
	while(1){
		r = uart_getc();
		if(r=='\n' || r=='\r' || r=='\0'){	// the end of input line
			break;
		}
		s[iter] = r;
		iter++;
	}
	s[iter] = '\0';
	if(s[0] == '\n'){
		uart_puts("FUCK\n");
	}
	return iter;
}

/**
 * Receive a string and display it character by character
 */
int uart_get_string_with_echo(char * s){
	char r;
	int iter = 0;
	while(1){
		r = uart_getc();
		if(r=='\n' || r=='\r' || r=='\0'){	// the end of input line
			uart_puts("\r\n");
			break;
		}
		uart_send(r);
		s[iter] = r;
		iter++;
	}
	s[iter] = '\0';
	return iter;
}

/**
 * Display a string
 */

void uart_puts(char *s) {
	for(int i = 0; s[i] != '\0'; i++) {
		uart_send((char)s[i]);
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

/**
 * for printf
 */
void putc(void *p, char c){
	uart_send(c);
}

void uart_int(int n){
	char s[1024];
	itoa(n, s, 10);
	uart_puts(s);
}
