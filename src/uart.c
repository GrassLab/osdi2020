#include "util.h"
#include "uart.h"
#include "mbox.h"

#ifdef MINIUART
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
#else
//#include "mbox.h"

void uart_send(char c){
    // add !?
    while((get32(UART_FR) & 0x20)) __asm__ volatile("nop");
	put32(UART_DR, c);
}

char uart_recv(void){
    while((get32(UART_FR) & 0x10)) __asm__ volatile("nop");
	return get32(UART_DR) & 0xFF;
}

void uart_flush(void){
    while( (get32(UART_FR) & 0x01)) 
         get32(UART_DR);
}

void uart_send_string(char* str){
    //while(*str) uart_send((char)*str), str++;
    do{(get32(UART_DR));}while(!(get32(UART_FR)&0x10));
}

//#define MHz 		*1000000
//#define UART_CLK	48 MHz
//#define UART_BAUD_RATE	115200
//#define UART_BAUD_DIV	((double)UART_CLK)/(16*UART_BAUD_RATE)
//#define UART_IBRD 	(unsigned int)(UART_BAUD_DIV)
//#define UART_FBRD	(unsigned int)((UART_BAUD_DIV - UART_IBRD)*64 + .5)

//void uart_init ( void )
//{
//	unsigned int selector;
//
//	selector = get32(GPFSEL1);
//	selector &= ~(7<<12);                   // clean gpio14
//	selector |= 4<<12;                      // set alt5 for gpio14
//	selector &= ~(7<<15);                   // clean gpio15
//	selector |= 4<<15;                      // set alt5 for gpio15
//	put32(GPFSEL1,selector);
//
//	put32(GPPUD,0);
//	delay(150);
//	put32(GPPUDCLK0,(1<<14)|(1<<15));
//	delay(150);
//	put32(GPPUDCLK0,0);
//
//	//At startup uart is disabled by default	
//	put32(UART_IBRD, UART_IBRD);         //Change baudrate in peripherals/uart_pl011.h
//	put32(UART_FBRD, UART_FBRD); 
//	put32(UART_LCRH, 0x70);              //Uart hardcoded for for 8 bit mode - no parity - fifo enable
//	put32(UART_CR, 0x301);             //Enable uart, RX and TX
//}

//void uart_init ( void )
//{
//	unsigned int selector;
//
//	selector = get32(GPFSEL1);
//	selector &= ~(7<<12);                   // clean gpio14
//	selector |= 4<<12;                      // set alt0 for gpio14
//	selector &= ~(7<<15);                   // clean gpio15
//	selector |= 4<<15;                      // set alt0 for gpio15
//	put32(GPFSEL1,selector);
//
//	put32(GPPUD,0);
//	delay(150);
//	put32(GPPUDCLK0,(1<<14)|(1<<15));
//	delay(150);
//	put32(GPPUDCLK0,0);
//
//	put32(UART_CR,0);						// disable RX and TX to configure
//	
//	put32(UART_IBRD,26);					//PrimeCell UART (PL011) rev.r1p5 pag.3-9 BAUDDIV = (FUARTCLK/(16 Baud rate)) = 48MHz/(16*115200) = 26.041666
//	put32(UART_FBRD,3);					
//	put32(UART_LCRH,0x60);	      			//Enable 8 bit mode
//
//	put32(UART_CR,0x301);					// enable UART, RX and TX
//}

void uart_init(void){
    unsigned int reg;
    put32(UART_CR, 0);

    unsigned int mbox[9];
    mbox[0] = 9 * 4;
    mbox[1] = REQUEST_CODE;
    mbox[2] = TAG_SETCLKRATE;
    mbox[3] = 12;
    mbox[4] = 8;
    mbox[5] = 2;           
    mbox[6] = 4000000;     
    mbox[7] = 0;           
    mbox[8] = END_TAG;
    mbox_call(mbox, MBOX_CH_PROP);

    reg = get32(GPFSEL1);
    reg &= ~((7<<12) | (7<<15)); // gpio14, gpio15
    reg |= (4<<12) | (4<<15);    // alt0
	put32(GPFSEL1, reg);
    put32(GPPUD, 0);
	delay(150);
	put32(GPPUDCLK0, (1<<14) | (1<<15));
	delay(150);
	put32(GPPUDCLK0, 0);
    put32(UART_ICR, 0x7FF);
    put32(UART_IBRD, 2);
    put32(UART_FBRD, 0xB);
    put32(UART_LCRH, 0b11 << 5);
    put32(UART_CR, 0x301);
}
#endif
