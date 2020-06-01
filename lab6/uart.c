#include "gpio.h"

// define mini UART registers
#define AUX_ENABLE	((volatile unsigned int*)(MMIO_BASE+0x00215004))
#define AUX_MU_IO	((volatile unsigned int*)(MMIO_BASE+0x00215040))
#define AUX_MU_IER	((volatile unsigned int*)(MMIO_BASE+0x00215044))
#define AUX_MU_IIR	((volatile unsigned int*)(MMIO_BASE+0x00215048))
#define AUX_MU_LCR	((volatile unsigned int*)(MMIO_BASE+0x0021504C))
#define AUX_MU_MCR	((volatile unsigned int*)(MMIO_BASE+0x00215050))
#define AUX_MU_LSR	((volatile unsigned int*)(MMIO_BASE+0x00215054))
#define AUX_MU_MSR	((volatile unsigned int*)(MMIO_BASE+0x00215058))
#define AUX_MU_SCRATCH	((volatile unsigned int*)(MMIO_BASE+0x0021505C))
#define AUX_MU_CNTL	((volatile unsigned int*)(MMIO_BASE+0x00215060))
#define AUX_MU_STAT	((volatile unsigned int*)(MMIO_BASE+0x00215064))
#define AUX_MU_BAUD	((volatile unsigned int*)(MMIO_BASE+0x00215068))

void
uart_init() 
{
	register unsigned int r;

	// initialize UART
	*AUX_ENABLE |= 1;
	*AUX_MU_CNTL = 0;
	*AUX_MU_LCR = 3;
	*AUX_MU_MCR = 0;
	*AUX_MU_IER = 0;
	*AUX_MU_IIR = 0xc6;	// disable interrupt
	*AUX_MU_BAUD = 270;

	// map uart1 to gpio pins
	r = *GPFSEL1;
	r &= ~((7<<12) | (7<<15));
	r |= (2<<12)|(2<<15);
	*GPFSEL1 = r;
	*GPPUD = 0;
	r = 150;
	while (r--) {
		asm volatile("nop");
	}
	*GPPUDCLK0 = (1<<14) | (1<<15);
	r = 150;
	while (r--) {
		asm volatile("nop");
	}
	*GPPUDCLK0 = 0;
	*AUX_MU_CNTL = 3;
}

void
uart_send(unsigned int c)
{
	do {
		asm volatile("nop");
	} while (!(*AUX_MU_LSR&0x20));
	*AUX_MU_IO = c;
}

char
uart_getc()
{
	char r;
	do {
		asm volatile("nop");
	} while (!(*AUX_MU_LSR&0x01));

	r = (char)(*AUX_MU_IO);
	return r=='\r'? '\n' : r;
}

void
uart_puts(char *s)
{
	while(*s) {
		if(*s == '\n') uart_send('\r');
		uart_send(*s++);
	}
}
