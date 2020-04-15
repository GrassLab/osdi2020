#include "gpio.h"
#include "shell.h"


#define AUX_ENABLE      ((volatile unsigned int*)(MMIO_BASE+0x00215004))
#define AUX_MU_IO       ((volatile unsigned int*)(MMIO_BASE+0x00215040))
#define AUX_MU_IER      ((volatile unsigned int*)(MMIO_BASE+0x00215044))
#define AUX_MU_IIR      ((volatile unsigned int*)(MMIO_BASE+0x00215048))
#define AUX_MU_LCR      ((volatile unsigned int*)(MMIO_BASE+0x0021504C))
#define AUX_MU_MCR      ((volatile unsigned int*)(MMIO_BASE+0x00215050))
#define AUX_MU_LSR      ((volatile unsigned int*)(MMIO_BASE+0x00215054))
#define AUX_MU_MSR      ((volatile unsigned int*)(MMIO_BASE+0x00215058))
#define AUX_MU_SCRATCH  ((volatile unsigned int*)(MMIO_BASE+0x0021505C))
#define AUX_MU_CNTL     ((volatile unsigned int*)(MMIO_BASE+0x00215060))
#define AUX_MU_STAT     ((volatile unsigned int*)(MMIO_BASE+0x00215064))
#define AUX_MU_BAUD     ((volatile unsigned int*)(MMIO_BASE+0x00215068))

void uart_init() {

    register unsigned int r;

    *AUX_ENABLE |= 1;
    *AUX_MU_CNTL = 0;
    *AUX_MU_IER = 0;
    *AUX_MU_LCR = 3;
    *AUX_MU_IIR = 0xc6;
    *AUX_MU_MCR = 0;
    *AUX_MU_BAUD = 270;

    r = *GPFSEL1;
    r &= ~((7<<12)|(7<<15));
    r |= ((2<<12)|(2<<15));

    *GPFSEL1 = r;
    *GPPUD = 0;

    r = 150; while(r--) { asm volatile("nop"); }
    *GPPUDCLK0 = (1<<14)|(1<<15);
    r = 150; while(r--) { asm volatile("nop"); }
    *GPPUDCLK0 = 0;
    *AUX_MU_CNTL = 3;
}

void uart_send(unsigned int c) {
    do{ asm volatile("nop"); }while( !(*AUX_MU_LSR&0x20) );
    *AUX_MU_IO = c;
}

char uart_getc() {
    char r;
    do{ asm volatile("nop"); }while( !(*AUX_MU_LSR&0x01) );
    r = (char)(*AUX_MU_IO);
    if(r == '\r') {
        r = '\n';
    }
    return r;
}



void uart_puts(char *s) {
    while( *s ) {
        if(*s == '\n') {
            uart_send('\r');
        }
        //prioity of * is lower than suffix ++.... 
        uart_send(*s++);
    }
}

void uart_hex(unsigned long d) {
    unsigned long n;
    int c;
    for(c=28;c>=0;c-=4) {
        // get highest tetrad
        n=(d>>c)&0xF;
        // 0-9 => '0'-'9', 10-15 => 'A'-'F'
        n+=n>9?0x37:0x30;
        uart_send(n);
    }
}

void write_file(char* adr) {
    int delay = 0;
    char message[64] = "";
    char r;
    char* ptr = adr;
    char bytes_count = 0;
    do{ asm volatile("nop"); }while( !(*AUX_MU_LSR&0x01) );
    r = (char)(*AUX_MU_IO);
    *ptr++ = r;
    while(delay < 1000000) {
        delay = 0;
        do {
            asm volatile("nop");
            delay++;
        } while( !(*AUX_MU_LSR&0x01) && delay < 1000000);
        if(delay < 1000000) {
            r = (char)(*AUX_MU_IO);
            *ptr++ = r;
            bytes_count++;
        } else {
            break;
        }
    }
    uart_puts("\n Stop reciving file due to can't ready file from serial port for a while.\r");
    num_string(bytes_count, message, 10);
    uart_puts("\nKernel size: ");
    uart_puts(message);
    uart_puts("\r");
    uart_puts("\nKernel address end at: ");
    uart_hex((unsigned long)ptr);
}
