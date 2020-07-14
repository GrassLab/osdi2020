#include "mailbox.h"

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

void set_mbox(unsigned int a)
{
    mbox[0] = 8 * 4;
	mbox[1] = MBOX_REQUEST;
	mbox[2] = a; // tag identifier
	mbox[3] = 8; // buffer size
	mbox[4] = MBOX_REQUEST; 
	mbox[5] = 0; // clear buffer data
	mbox[6] = 0; 
    mbox[7] = MBOX_TAG_LAST; 
}

void get_board_revision(void)
{
	set_mbox(MBOX_TAG_GET_BOARD_REVISION);
	if(mbox_call(MBOX_CH_PROP))
    {
        uart_puts("My board_revision number is\n");	
        uart_hex(mbox[5]);
        uart_puts("\n");
    }
    else
    {
        uart_puts("Unable to query serial!\n");
    }
    
}
void get_serial(void)
{
	set_mbox(MBOX_TAG_GETSERIAL);
	if(mbox_call(MBOX_CH_PROP))
    {
        uart_puts("My serial number is\n");	
        uart_hex(mbox[5]);
        uart_puts("\n");
    }
    else
    {
        uart_puts("Unable to query serial!\n");
    }
}

void get_vc_information(void)
{
	set_mbox(MBOX_TAG_GET_VC);
	if(mbox_call(MBOX_CH_PROP))
    {
        uart_puts("My VC base address is\n");	
        uart_hex(mbox[5]);
        uart_puts("\n");
        uart_puts("My VC memory size is\n");	
        uart_hex(mbox[6]);
        uart_puts("\n");
    }
    else
    {
        uart_puts("Unable to query serial!\n");
    }
}

void get_arm_information(void)
{
	set_mbox(MBOX_TAG_GET_ARM);
	if(mbox_call(MBOX_CH_PROP))
    {
        uart_puts("My ARM base address is\n");	
        uart_hex(mbox[5]);
        uart_puts("\n");
        uart_puts("My ARM memory size is\n");	
        uart_hex(mbox[6]);
        uart_puts("\n");
    }
    else
    {
        uart_puts("Unable to query serial!\n");
    }
}

void set_uart0_clock_rate(void)
{
    /* set up clock for consistent divisor values */
    mbox[0] = 9*4;
    mbox[1] = MBOX_REQUEST;
    mbox[2] = MBOX_TAG_SETCLKRATE; // set clock rate
    mbox[3] = 12;
    mbox[4] = 8;
    mbox[5] = 2;           // UART clock
    mbox[6] = 4000000;     // 4Mhz
    mbox[7] = 0;           // clear turbo
    mbox[8] = MBOX_TAG_LAST;
}