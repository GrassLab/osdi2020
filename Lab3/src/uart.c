#include "include/mbox.h"
#include "include/peripherals/uart.h"
#include "include/peripherals/gpio.h"
#include "include/utils.h"

#define IRQ_ENABLE1 0x3f00b214

void uart_init(){

    /* initialize UART */
    put32(UART0_CR,0);         // turn off UART0

    /* set up clock for consistent divisor values */
    mbox[0] = 9*4;
    mbox[1] = REQUEST_CODE;
    mbox[2] = MBOX_TAG_SETCLKRATE; // set clock rate
    mbox[3] = 12;          // request length
    mbox[4] = 8;           // response length
    mbox[5] = 2;           // UART clock id
    mbox[6] = 4000000;     // rate = 4Mhz
    mbox[7] = 0;           // clear turbo
    mbox[8] = END_TAG;
    mbox_call(8);
    
    unsigned int selector;

    selector = get32(GPFSEL1);
    selector &= ~(7<<12); // clean gpio 14
    selector |= 4<<12;    // set alt0 for gpio4
    selector &= ~(7<<15); // clean gpio 15
    selector |= (4<<15);  // set alt0 for gpio5 
    put32(GPFSEL1,selector);

    put32(GPPUD,0);
    delay(150);
    put32(GPPUDCLK0,(1<<14)|(1<<15));
    delay(150);
    put32(GPPUDCLK0,0);

 
    put32(UART0_ICR,0x7FF); //clear interrupt
    // init uart0 interrupt
    put32(UART0_IMSC,0x30); // interrupt mask
    put32(IRQ_ENABLE1,1<<25);
    
    put32(UART0_IBRD,2); // (4 × 10^6) / (16 × 115200) = 2.17
    put32(UART0_FBRD,0xB); // int((0.17 × 64) + 0.5) = 11
    put32(UART0_LCRH,0b11<<5); //word length = 8bits
    put32(UART0_CR,0x301); // enable Tx,Rx,FIFO    
}


void uart_send ( char c )
{
	while(get32(UART0_FR)&0x20){
	}
	put32(UART0_DR,c);
}

char uart_recv ()
{
	while(get32(UART0_FR)&0x10) {
		
	}
	return(get32(UART0_DR)&0xFF);
}

void uart_send_string(char* str)
{
	for (int i = 0; str[i] != '\0'; i ++) {
		uart_send((char)str[i]);
	}

}

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

void uart_IRQhandler(){
	unsigned int status = get32(UART0_MIS);	
	
	if(status&0x10){ // for receive

		put32(UART0_ICR,status); //clear interrupt

	}
	else{	
		put32(UART0_ICR,status); //clear interrupt
	}
}
