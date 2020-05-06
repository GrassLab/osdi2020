#include "gpio.h"
#include "mbox.h"
#include "uart.h"

/* mailbox message buffer */
volatile unsigned int  __attribute__((aligned(16))) mbox[36];

#define VIDEOCORE_MBOX  (MMIO_BASE+0x0000B880)
#define MBOX_READ       ((volatile unsigned int*)(VIDEOCORE_MBOX+0x0))
#define MBOX_POLL       ((volatile unsigned int*)(VIDEOCORE_MBOX+0x10))
#define MBOX_SENDER     ((volatile unsigned int*)(VIDEOCORE_MBOX+0x14))
#define MBOX_STATUS     ((volatile unsigned int*)(VIDEOCORE_MBOX+0x18))
#define MBOX_CONFIG     ((volatile unsigned int*)(VIDEOCORE_MBOX+0x1C))
#define MBOX_WRITE      ((volatile unsigned int*)(VIDEOCORE_MBOX+0x20))
#define MBOX_RESPONSE   0x80000000
#define MBOX_FULL       0x80000000
#define MBOX_EMPTY      0x40000000

/**
 * Make a mailbox call. Returns 0 on failure, non-zero on success
 */
int mbox_call(unsigned char ch)
{
    unsigned int r = (((unsigned int)((unsigned long)&mbox)&~0xF) | (ch&0xF));
    /* wait until we can write to the mailbox */
    do{asm volatile("nop");}while(*MBOX_STATUS & MBOX_FULL);
    /* write the address of our message to the mailbox with channel identifier */
    *MBOX_WRITE = r;
    /* now wait for the response */
    while(1) {
        /* is there a response? */
        do{asm volatile("nop");}while(*MBOX_STATUS & MBOX_EMPTY);
        /* is it a response to our message? */
        if(r == *MBOX_READ)
            /* is it a valid successful response? */
            return mbox[1]==MBOX_RESPONSE;
    }
    return 0;
}

void get_serial_number(){
	
	// get the board's unique serial number with a mailbox call
    mbox[0] = 8*4;                  // length of the message
    mbox[1] = MBOX_REQUEST;         // this is a request message
    
    mbox[2] = MBOX_TAG_GETSERIAL;   // get serial number command
    mbox[3] = 8;                    // buffer size
    mbox[4] = 8;
    mbox[5] = 0;                    // clear output buffer
    mbox[6] = 0;

    mbox[7] = MBOX_TAG_LAST;

    // send the message to the GPU and receive answer
    if (mbox_call(MBOX_CH_PROP)) {
        uart_puts("My serial number is: ");
        uart_hex(mbox[5]);
        uart_hex(mbox[6]);
		uart_puts("\n");
    } else {
        uart_puts("Unable to query serial!\n");
    }
}

void get_board_revision(){
	
	// set the mailbox getting board revision
	mbox[0] = 7*4;                  // length of the message
    mbox[1] = MBOX_REQUEST;         // this is a request message 
    mbox[2] = MBOX_TAG_GETBOARDREVISION;   // get board revision number command
    mbox[3] = 4;                    // buffer size
    mbox[4] = 8;
    mbox[5] = 0;                    // clear output buffer
    mbox[6] = MBOX_TAG_LAST;

    // send the message to the GPU and receive answer
    if (mbox_call(MBOX_CH_PROP)) {
        uart_puts("My board revision is: ");
        uart_hex(mbox[5]);
        uart_puts("\n");
    } else {
        uart_puts("Unable to query board revision!\n");
    }
}

void get_VC_memory(){
	
	// set the mailbox for getting VC Memory
	mbox[0] = 8*4;                  // length of the message
    mbox[1] = MBOX_REQUEST;         // this is a request message 
    mbox[2] = MBOX_TAG_GETVCMEMORYADDR;   // get VC memory base address command
    mbox[3] = 8;                    // buffer size
    mbox[4] = 8;
    mbox[5] = 0;                    // clear output buffer
    mbox[6] = 0;
    mbox[7] = MBOX_TAG_LAST;

    // send the message to the GPU and receive answer
	if (mbox_call(MBOX_CH_PROP)) {
        uart_puts("My VC memory base address is: ");
        uart_hex(mbox[5]);
		uart_puts("\n");
		uart_puts("My VC memory size is: ");
        uart_hex(mbox[6]);
        uart_puts("\n");
    } else {
        uart_puts("Unable to query VC memory address!\n");
    }
}
