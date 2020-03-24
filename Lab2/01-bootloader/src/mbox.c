#include "include/mbox.h"
#include "include/peripherals/mbox.h"
#include "include/utils.h"

// make sure buffer is properly aligned !!
unsigned int __attribute__((aligned(16))) mbox[36];

int mbox_call(unsigned char ch){
	//Combine the message address(upper 28bits) 
	// with channel number(lower 4bits)	
	unsigned int r = (((unsigned int)((unsigned long)&mbox)&~0xF) | (ch&0xF));	
	// This comment is used to check if mbox has wrong address
	//uart_hex((unsigned long)&mbox);
	
	// check if Mailbox 0 status register's full flag is set
	while(get32(MAILBOX_STATUS) & MAILBOX_FULL){}
	// if not, write mailbox1 read/write reg
	put32(MAILBOX_WRITE,r);

	while(1){
		// check if Mailbox 0 status register's empty flag is set
		while(get32(MAILBOX_STATUS) & MAILBOX_EMPTY);
		// if not, read from mailbox0 read_write reg
		if(r == get32(MAILBOX_READ))
			return mbox[1]==REQUEST_SUCCEED;
	}
	
	return 0;
}
