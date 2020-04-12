
#ifdef MINIUART
#include "gpio.h"
#include "util.h"

#define AUXENB          ((volatile unsigned int*)(MMIO_BASE + 0x00215004))
#define AUX_MU_CNTL_REG ((volatile unsigned int*)(MMIO_BASE + 0x00215060))
#define AUX_MU_IER_REG  ((volatile unsigned int*)(MMIO_BASE + 0x00215044))
#define AUX_MU_LCR_REG  ((volatile unsigned int*)(MMIO_BASE + 0x0021504C))
#define AUX_MU_MCR_REG  ((volatile unsigned int*)(MMIO_BASE + 0x00215050))
#define AUX_MU_BAUD_REG ((volatile unsigned int*)(MMIO_BASE + 0x00215068))
#define AUX_MU_IIR_REG  ((volatile unsigned int*)(MMIO_BASE + 0x00215048))
#define AUX_MU_CNTL_REG ((volatile unsigned int*)(MMIO_BASE + 0x00215060))
#define AUX_MU_IO_REG   ((volatile unsigned int*)(MMIO_BASE + 0x00215040))
#define AUX_MU_LSR_REG  ((volatile unsigned int*)(MMIO_BASE + 0x00215054))

void uart_send(char c){
    while(!(*(AUX_MU_LSR_REG) & 0x20)) __asm__ volatile("nop");
	*(AUX_MU_IO_REG) = c;
}

char uart_recv(void){
    while(!(*(AUX_MU_LSR_REG) & 0x01)) __asm__ volatile("nop");
	return *(AUX_MU_IO_REG) & 0xFF;
}

void uart_flush(void){
    while(*(AUX_MU_LSR_REG) & 0x01) 
        *(AUX_MU_IO_REG);
}

void uart_puts(char* str){
    while(*str) uart_send((char)*str), str++;
}

void uart_init(void){
    unsigned int reg;
    reg = *(GPFSEL1);
    reg &= ~(7<<12);  
	reg |= 2<<12;     
	reg &= ~(7<<15);  
	reg |= 2<<15;     
	*(GPFSEL1) = reg;

    *(GPPUD) = 0;
	delay(150);
	*(GPPUDCLK0) = (1<<14) | (1<<15);
	delay(150);
	*(GPPUDCLK0) = 0;
    *(AUXENB) = 1;                   
	*(AUX_MU_CNTL_REG) = 0;               
	*(AUX_MU_IER_REG) = 0;                
	*(AUX_MU_LCR_REG) = 3;                
	*(AUX_MU_MCR_REG) = 0;                
	*(AUX_MU_BAUD_REG) = 270;             
	*(AUX_MU_CNTL_REG) = 3;               
}
#endif
