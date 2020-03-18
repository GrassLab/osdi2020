#include "util.h"
#include "mini_uart.h"

void uart_send(char c){
    while(!(get32(AUX_MU_LSR_REG) & 0x20)) __asm__ volatile("nop");
	put32(AUX_MU_IO_REG, c);
}

char uart_recv(void){
    while(!(get32(AUX_MU_LSR_REG) & 0x01)) __asm__ volatile("nop");
	return get32(AUX_MU_IO_REG) & 0xFF;
}

void uart_flush(void){
    while(get32(AUX_MU_LSR_REG) & 0x01) 
        get32(AUX_MU_IO_REG);
}

void uart_send_string(char* str){
    while(*str) uart_send((char)*str), str++;
}

void uart_init(void){
    unsigned int reg;
    reg = get32(GPFSEL1);
    reg &= ~(7<<12);  
	reg |= 2<<12;     
	reg &= ~(7<<15);  
	reg |= 2<<15;     
	put32(GPFSEL1, reg);

    put32(GPPUD, 0);
	delay(150);
	put32(GPPUDCLK0, (1<<14) | (1<<15));
	delay(150);
	put32(GPPUDCLK0, 0);
    put32(AUXENB, 1);                   
	put32(AUX_MU_CNTL_REG, 0);               
	put32(AUX_MU_IER_REG, 0);                
	put32(AUX_MU_LCR_REG, 3);                
	put32(AUX_MU_MCR_REG, 0);                
	put32(AUX_MU_BAUD_REG, 270);             
	put32(AUX_MU_CNTL_REG, 3);               
}
