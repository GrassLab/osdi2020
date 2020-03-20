#include "include/mbox.h"
#include "include/peripherals/mbox.h"
#include "include/utils.h"
#include "include/mini_uart.h"
#include "include/string.h"

// make sure buffer is properly aligned !!
unsigned int __attribute__((aligned(16))) mbox[7];

int mbox_call(unsigned char ch){
	//Combine the message address(upper 28bits) 
	// with channel number(lower 4bits)	
	unsigned int a = (unsigned long)&mbox;
	unsigned int addr = (((unsigned int)(a)&~0xF) | (ch&0xF)) ;
	
	// This comment is used to check if mbox has wrong address
	//uart_hex((unsigned long)&mbox);
	
	// check if Mailbox 0 status register's full flag is set
	while(get32(MAILBOX_STATUS) & MAILBOX_FULL){}
	// if not, write mailbox1 read/write reg
	put32(MAILBOX_WRITE,addr);

	while(1){
		// check if Mailbox 0 status register's empty flag is set
		while(get32(MAILBOX_STATUS) & MAILBOX_EMPTY){}
		int mailbox_read = get32(MAILBOX_READ);
		// if not, read from mailbox0 read_write reg
		if(addr == mailbox_read){
			return mbox[1]==MBOX_RESPONSE;
		}
	}
	
	return 0;
}
