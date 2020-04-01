#include "hard_info.h"
#include "uart.h"

#define MMU_BASE            0x3F000000

#define GPFSEL1             ((volatile unsigned int*)(MMU_BASE+0x00200004))
#define GPPUD               ((volatile unsigned int*)(MMU_BASE+0x00200094))
#define GPPUDCLK0           ((volatile unsigned int*)(MMU_BASE+0x0020009C))

#define AUX_ENABLES         ((volatile unsigned int*)(MMU_BASE+0x00215004))
#define AUX_MU_CNTL_REG     ((volatile unsigned int*)(MMU_BASE+0x00215060))
#define AUX_MU_IER_REG      ((volatile unsigned int*)(MMU_BASE+0x00215044))
#define AUX_MU_LCR_REG      ((volatile unsigned int*)(MMU_BASE+0x0021504C))
#define AUX_MU_MCR_REG      ((volatile unsigned int*)(MMU_BASE+0x00215050))
#define AUX_MU_BAUD         ((volatile unsigned int*)(MMU_BASE+0x00215068))
#define AUX_MU_IIR_REG      ((volatile unsigned int*)(MMU_BASE+0x00215048))
#define AUX_MU_IO_REG       ((volatile unsigned int*)(MMU_BASE+0x00215040))
#define AUX_MU_LSR_REG      ((volatile unsigned int*)(MMU_BASE+0x00215054))


void uart_init(){
    register unsigned int p;
    register unsigned int r;
    *AUX_ENABLES|=1;        //enable uart1
    *AUX_MU_CNTL_REG=0;     //disable transmit
    *AUX_MU_IER_REG=0;      //disable interrupt
    *AUX_MU_LCR_REG=3;      //8bits
    *AUX_MU_MCR_REG=0;      //not auto flow control
    *AUX_MU_BAUD=270;       //baud rate 115200
    *AUX_MU_IIR_REG=0xc6;   //no fifo
    p=*GPFSEL1;             
    p&=~(63<<12);           //clear gpio14,15 alt
    p|=(2<<12)|(2<<15);     //alt5
    *GPFSEL1=p;
    *GPPUD=0;               //no pupd
    for(p=0;p<150;p++){
        asm volatile("nop");    
    }
    *GPPUDCLK0=(3<<14);
    for(p=0;p<150;p++){
        asm volatile("nop");    
    }
    *GPPUDCLK0=0;
    *AUX_MU_CNTL_REG=3;         //re-enable tansmit
}

char uart_read(){
    char a;
    asm volatile("nop");
    while(!(*AUX_MU_LSR_REG&0x01)){
        asm volatile("nop");
    }
    a=(char)(*AUX_MU_IO_REG);
    return a;
}

void uart_write(unsigned int c){
    asm volatile("nop");
    while(!(*AUX_MU_LSR_REG&0x20)){
        asm volatile("nop");    
    }
    *AUX_MU_IO_REG=c;
}


