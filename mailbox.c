#include "gpio.h"

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

#define GET_BOARD_REVISION  0x00010002
#define GET_VC_MEMORY	    0x00010006
#define SET_CLOCK_RATE      0x00038002

#define REQUEST_CODE        0x00000000
#define REQUEST_SUCCEED     0x80000000
#define REQUEST_FAILED      0x80000001
#define TAG_REQUEST_CODE    0x00000000
#define END_TAG             0x00000000

volatile unsigned int  __attribute__((aligned(16))) mailbox[36];

int mailbox_call(unsigned char channel)
{
    unsigned int r = (((unsigned int)((unsigned long)&mailbox)&~0xF) | ( channel &0xF));
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
            return mailbox[1]==MBOX_RESPONSE;
    }
    return 0;
}

void get_board_revision()
{
  mailbox[0] = 7 * 4; // buffer size in bytes
  mailbox[1] = REQUEST_CODE;
  // tags begin
  mailbox[2] = GET_BOARD_REVISION; // tag identifier
  mailbox[3] = 4; // maximum of request and response value buffer's length.
  mailbox[4] = TAG_REQUEST_CODE;
  mailbox[5] = 0; // value buffer
  // tags end
  mailbox[6] = END_TAG;

  mailbox_call(8); // message passing procedure call, you should implement it following the 6 steps provided above.
  
}

void get_vc_memory()
{
  mailbox[0] = 8 * 4; // buffer size in bytes
  mailbox[1] = REQUEST_CODE;
  // tags begin
  mailbox[2] = GET_VC_MEMORY; // tag identifier
  mailbox[3] = 8; // maximum of request and response value buffer's length.
  mailbox[4] = 8;
  mailbox[5] = 0; // value buffer
  mailbox[6] = 0;
  // tags end
  mailbox[7] = END_TAG;

  mailbox_call(8);
}

void set_clock_rate()
{
    mailbox[0] = 9*4;
    mailbox[1] = REQUEST_CODE;
    mailbox[2] = SET_CLOCK_RATE; // set clock rate
    mailbox[3] = 12;
    mailbox[4] = 8;
    mailbox[5] = 2;           // UART clock
    mailbox[6] = 4000000;     // 4Mhz
    mailbox[7] = 0;           // clear turbo
    mailbox[8] = END_TAG;
  
    mailbox_call(8);
}
