#include "uart.h"
#include "mailbox.h"
#define MMIO_BASE       0x3F000000
#define MAILBOX_BASE    ((MMIO_BASE+0x0000B880))
#define MAILBOX_READ    ((volatile unsigned int*)(MAILBOX_BASE))
#define MAILBOX_STATUS  ((volatile unsigned int*)(MAILBOX_BASE+0x00000018))
#define MAILBOX_WRITE   ((volatile unsigned int*)(MAILBOX_BASE+0x00000020))

volatile unsigned int mailbox[36];

int mailbox_call(){
	int flag=0;
	unsigned int r= ((unsigned int)mailbox)+8;
	do{asm volatile("nop");}while(*MAILBOX_STATUS & MAILBOX_FULL);
	*MAILBOX_WRITE = r;
	while(1){
		do{asm volatile("nop");}while(*MAILBOX_STATUS & MAILBOX_EMPTY);
		if(r == *MAILBOX_READ)break;
		else ++flag;
		if(flag%1000000==0)uart_puts("incorrect\n",0);
	}
	uart_puts("success\n",0);
	return 1;
	//uart_hex(mailbox[5]);
}
void get_board_revision(){
	mailbox[0] = 7 * 4;
  	mailbox[1] = REQUEST_CODE;
  	mailbox[2] = GET_BOARD_REVISION;
  	mailbox[3] = 4;
  	mailbox[4] = TAG_REQUEST_CODE;
  	mailbox[5] = 0;
	mailbox[6] = END_TAG;
  	if(mailbox_call()){
		uart_puts("vboard revision: ",17);
		uart_hex(mailbox[5]);
	}
	else
		uart_puts("Fail",4);
	uart_send('\r');
	uart_send('\n');
	return;
}
void get_vc_memory(){
	mailbox[0] = 8 * 4;
  	mailbox[1] = REQUEST_CODE;
  	mailbox[2] = GET_VC_MEMORY;
  	mailbox[3] = 8;
  	mailbox[4] = TAG_REQUEST_CODE;
  	mailbox[5] = 0;
	mailbox[6] = 0;
	mailbox[7] = END_TAG;
  	if(mailbox_call()){
		uart_puts("vc core base addr: ",19);
		uart_hex(mailbox[5]);
		uart_puts("; vc core mem size: ",21);
		uart_hex(mailbox[6]);	
		uart_send('\r');
		uart_send('\n');
	}
	else
		uart_puts("Fail",4);
	uart_send('\r');
	uart_send('\n');
	return;
}
