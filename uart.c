#include "uart.h"
#define AUX_REG_BASE	0x3F215000
#define AUXENB		((volatile unsigned int*)(AUX_REG_BASE+0x00000004))
#define AUX_MU_CNTL_REG	((volatile unsigned int*)(AUX_REG_BASE+0x00000060))
#define AUX_MU_IER_REG	((volatile unsigned int*)(AUX_REG_BASE+0x00000044))
#define AUX_MU_LCR_REG	((volatile unsigned int*)(AUX_REG_BASE+0x0000004C))
#define AUX_MU_MCR_REG	((volatile unsigned int*)(AUX_REG_BASE+0x00000050))
#define AUX_MU_BAUD	((volatile unsigned int*)(AUX_REG_BASE+0x00000068))
#define AUX_MU_IIR_REG	((volatile unsigned int*)(AUX_REG_BASE+0x00000048))
#define AUX_MU_LSR_REG	((volatile unsigned int*)(AUX_REG_BASE+0x00000054))
#define AUX_MU_IO_REG	((volatile unsigned int*)(AUX_REG_BASE+0x00000040))

#define GPFSEL1		((volatile unsigned int*)(0x3F200004))
#define GPPUD		((volatile unsigned int*)(0x3F200094))
#define GPPUDCLK0	((volatile unsigned int*)(0x3F200098))
#define WAITING_CYCLE 	150


void uart_init(){
	register unsigned int r;
	*AUXENB = 1;
	*AUX_MU_CNTL_REG = 0;
	*AUX_MU_IER_REG = 0;
	*AUX_MU_LCR_REG = 3;
	*AUX_MU_MCR_REG = 0;
	*AUX_MU_BAUD = 270;
	*AUX_MU_IIR_REG = 6;

	r=*GPFSEL1;
	r&=~((7<<12)|(7<<15));
	r|=(2<<12)|(2<<15);
	*GPFSEL1 = r;
	*GPPUD = 0;
	r=WAITING_CYCLE;
	while(r--){asm volatile("nop");}
	*GPPUDCLK0 = (1<<14)|(1<<15);
	r=WAITING_CYCLE;
	while(r--){asm volatile("nop");}
	*GPPUDCLK0 = 0;

	*AUX_MU_CNTL_REG = 3;

}

void uart_send(unsigned int c){
	do{asm volatile("nop");}while(!(*AUX_MU_LSR_REG&0x20));
	*AUX_MU_IO_REG=c;
}

char uart_getc(){
	char r;
	do{asm volatile("nop");}while(!(*AUX_MU_LSR_REG&0x01));
	r=(char)(*AUX_MU_IO_REG);
	return r=='\r'?'\n':r;
}

void uart_puts(char *s, int n){//0 with newline, or n chars without newline
	int i=0;
	if(n){
		while(i++<n)uart_send(*s++);
		return;
	}
	while(*s){
		if(*s=='\n')uart_send('\r');
		uart_send(*s++);
	}
}
void uart_send_int(unsigned long num){
	char rev[20],s[20];
	int cnt=0,i=0;
	rev[0]='0';
	while(num){
		rev[cnt++]=(num%10)+'0';
		num/=10;
	}
//	uart_puts(rev,cnt);
	while(i<cnt){
		s[i]=rev[cnt-i-1];
		i++;
	}
	uart_puts(s,cnt);
}
