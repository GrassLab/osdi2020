#include "utils.h"
#include "peripherals/uart.h"
#include "peripherals/gpio.h"
#include "mbox.h"
#include "gpio.h"
/* PL011 UART registers */
#define UART0_DR        ((volatile unsigned int*)(MMIO_BASE+0x00201000))
#define UART0_FR        ((volatile unsigned int*)(MMIO_BASE+0x00201018))
#define UART0_IBRD      ((volatile unsigned int*)(MMIO_BASE+0x00201024))
#define UART0_FBRD      ((volatile unsigned int*)(MMIO_BASE+0x00201028))
#define UART0_LCRH      ((volatile unsigned int*)(MMIO_BASE+0x0020102C))
#define UART0_CR        ((volatile unsigned int*)(MMIO_BASE+0x00201030))
#define UART0_IMSC      ((volatile unsigned int*)(MMIO_BASE+0x00201038))
#define UART0_ICR       ((volatile unsigned int*)(MMIO_BASE+0x00201044))

void reserve(char *str,int index)
{
    int i = 0, j = index - 1, temp;
    while (i < j) {
        temp = str[i]; 
        str[i] = str[j]; 
        str[j] = temp;
        i++;
        j--;
    }
}

void uart_init()
{
    register unsigned int r;

    /* initialize UART */
    *UART0_CR = 0;         // turn off UART0

    /* set up clock for consistent divisor values */
    mbox[0] = 9*4;
    mbox[1] = REQUEST_CODE;
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

void uart_send(char c) {
    // while transmit FIFO is full (page 181)
    while (get32(UART_FR) & (1 << 5));
    put32(UART_DR, c);
}

char uart_recv() {
    // while the Receive FIFO is empty (page 181)
    while (get32(UART_FR) & (1 << 4));
    return get32(UART_DR)&0xFF;
}

void uart_send_string(char *str) {
    for (char c = *str; c != '\0'; c = *(++str)) {
        uart_send(c);
    }
}

int readline(char *buf, int maxlen) {
    int num = 0;
    while (num < maxlen - 1) {
        char c = uart_recv();
        if (c == '\n' || c == '\0' || c == '\r') {
            break;
        }
        buf[num] = c;
        num++;
    }
    buf[num] = '\0';
    return num;
}

int uart_read_int() {
    int num = 0, index, i;
    char buff[100];
    index = readline(buff, 100);
    for (i = 0 ; i < index; i++) {
        num = num * 10;
        num += buff[i] - '0';
    }
    return num;
}


void uart_send_int(int number) {
    int i = 0, j; 
    char str[100];
    while (number) { 
        str[i++] = (number % 10) + '0'; 
        number = number / 10; 
    } 
    reserve(str, i);
    for ( j = 0 ; j < i ; j++) {
        uart_send(str[j]);
    }
    uart_send('\n');
    return ; 
}

void uart_send_hex(unsigned long number) {
    char buffer[11];
    int i;
    buffer[0] = '0';
    buffer[1] = 'x'; 
    buffer[2] = (char)((number >> 28) & 0xF);
    buffer[3] = (char)((number >> 24) & 0xF);
    buffer[4] = (char)((number >> 20) & 0xF);
    buffer[5] = (char)((number >> 16) & 0xF);
    buffer[6] = (char)((number >> 12) & 0xF);
    buffer[7] = (char)((number >> 8) & 0xF);
    buffer[8] = (char)((number >> 4) & 0xF);
    buffer[9] = (char)(number & 0xF);
    for (i = 2 ; i < 10 ; i++) {
        if (buffer[i] < 10) {
            buffer[i] += 48;
        }
        else {
            buffer[i] += 55;
        }
    }
    buffer[10] = '\n';
    buffer[11] = '\0';
    uart_send_string(buffer);
    return;
}

long long int read_kernel_address() 
{
    char buff[10];
    int index, i;
    long long int kernel_address = 0;
    index = readline(buff, 10);
    for (i = 0 ; i < index; i++) {
        kernel_address = kernel_address * 10;
        kernel_address += buff[i] - '0';
    }
    return kernel_address;
}