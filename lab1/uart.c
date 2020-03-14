#include "gpio.h"
#include "sprintf.h"
/* Auxiliary mini UART regs*/
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

/* get addr from linker */
extern volatile unsigned char _end;

/**
 * Set baud rate, characteristics and map to GPIO
 */
void uart_init()
{
    register unsigned int reg_tmp;

    /* init UART */
    *AUX_ENABLE |= 1;   // enable mini uart, and then can access uart regs
    *AUX_MU_CNTL = 0;   // disable transmitter & receiver during configuration
    *AUX_MU_IER = 0;    // disable interrupt
    *AUX_MU_IIR = 0xc6; // clear FIFO
    *AUX_MU_LCR = 3;    // set data size 8 bits
    *AUX_MU_MCR = 0;    // disable auto flow control
    *AUX_MU_BAUD = 270; // 115200 baud
    
    /* clear GPIO 14, 15 pins for alternative function */
    reg_tmp = *GPFSEL1;
    reg_tmp &= ~(7 << 12); // clean GPIO14
    reg_tmp |= 2 << 12;    // set alt5 for GPIO14   
    reg_tmp &= ~(7 << 15); // clean GPIO15
    reg_tmp |= 2 << 15;    // set alt5 for GPIO15
    *GPFSEL1 = reg_tmp;

    /* disable poll-up/down mechanism, cause UART is always connecting */
    *GPPUD = 0;
    /* delay 150 cycle */
    reg_tmp = 150;
    while(reg_tmp--) { asm volatile("nop"); }
    *GPPUDCLK0 = (1 << 14) | (1 << 15); // modify clock change
    reg_tmp = 150;
    while(reg_tmp--) {asm volatile("nop"); }
    *GPPUDCLK0 = 0;
    
    *AUX_MU_CNTL = 3;  // enable receiver & transmitter
}

/**
 * Send a char
 **/
void uart_sendc(unsigned int c)
{
    while(!(*AUX_MU_LSR & 0x20)) {}
    /* write a char to buf */
    *AUX_MU_IO = c; 
}

/**
 * Get a char
 **/
char uart_getc()
{
    char c;
    while(!(*AUX_MU_LSR & 0x01)) {}
    c = (char)(*AUX_MU_IO);
    /* convert carrige return to newline */
    return c == '\r' ? '\n' : c; 
}

/**
 * Display a string
 **/
void uart_puts(char *s)
{
    while(*s) {
        /* convert newline to '\r\n' */
        if(*s == '\n') 
            uart_sendc('\r');
        uart_sendc(*s++);
    }
}

/**
 * Display a string with libc printf-like format
 */
void printf(char *fmt, ...) {
    __builtin_va_list args;
    __builtin_va_start(args, fmt);
    
    /* s don't allocate memory, just place string after code */
    char *s = (char*)&_end;
    /* format string */
    vsprintf(s, fmt, args);
    /* print string */
    while(*s) {
        if(*s == '\n') uart_sendc('\r');
        uart_sendc(*s++);
    }

}

