#include "gpio.h"
#include "printf.h"
#include "uart.h"

// #define MMIO_BASE       0x3f000000
#define MAILBOX_BASE    MMIO_BASE + 0xb880

#define MAILBOX_READ    ((volatile unsigned int*)(MAILBOX_BASE))
#define MAILBOX_STATUS  ((volatile unsigned int*)(MAILBOX_BASE + 0x18))
#define MAILBOX_WRITE   ((volatile unsigned int*)(MAILBOX_BASE + 0x20))

#define MAILBOX_RESPONSE    0x80000000
#define MAILBOX_EMPTY       0x40000000
#define MAILBOX_FULL        0x80000000

#define GET_BOARD_REVISION  0x00010002
#define GET_SERIAL_NUMBER   0x00010004
#define GET_VC_MEMORY       0x00010006

#define REQUEST_CODE        0x00000000
#define REQUEST_SUCCEED     0x80000000
#define REQUEST_FAILED      0x80000001
#define TAG_REQUEST_CODE    0x00000000
#define END_TAG             0x00000000

int mailbox_call(unsigned int *mbox, unsigned char ch){
    unsigned int address = (((unsigned int)((unsigned long)mbox)&~0xF) | (ch&0xF));

    /* wait until we can write to the mailbox */
    do{
      asm volatile("nop");
    }while(*MAILBOX_STATUS & MAILBOX_FULL);

    /* write the address of our message to the mailbox with channel identifier */
    *MAILBOX_WRITE = address;

    /* now wait for the response */
    while(1) {
        /* is there a response? */
        do{
          asm volatile("nop");
        }while(*MAILBOX_STATUS & MAILBOX_EMPTY);

        // printf("%d %d\n",address, *MAILBOX_READ);
        /* is it a response to our message? */
        if(address == *MAILBOX_READ)
            /* is it a valid successful response? */
            return *(mbox+1)==MAILBOX_RESPONSE;
    }
    return 0;
}

void get_board_revision(){
    unsigned int mailbox[7];
    mailbox[0] = 7 * 4; // buffer size in bytes
    mailbox[1] = REQUEST_CODE;
    // tags begin
    mailbox[2] = GET_BOARD_REVISION; // tag identifier
    mailbox[3] = 4; // maximum of request and response value buffer's length.
    mailbox[4] = TAG_REQUEST_CODE;
    mailbox[5] = 0; // value buffer
    // tags end
    mailbox[6] = END_TAG;

    // message passing procedure call, you should implement it following the 6 steps provided above.
    if(mailbox_call(mailbox, 8)){
        printf("Board revision: 0x%x\n", mailbox[5]); // it should be 0xa020d3 for rpi3 b+
    }
}

void get_VC_memory(){
    unsigned int mailbox[8];
    mailbox[0] = 8 * 4; // buffer size in bytes
    mailbox[1] = REQUEST_CODE;
    // tags begin
    mailbox[2] = GET_VC_MEMORY; // tag identifier
    mailbox[3] = 8; // maximum of request and response value buffer's length.
    mailbox[4] = TAG_REQUEST_CODE;
    mailbox[5] = 0; // value buffer
    mailbox[6] = 0;
    // tags end
    mailbox[7] = END_TAG;

    // message passing procedure call, you should implement it following the 6 steps provided above.
    if(mailbox_call(mailbox, 8)){
        printf("VC Core base addr: 0x%x\n", mailbox[5]);
        printf("VC memory size: 0x%x\n", mailbox[6]);
    }else{
        printf("Unable to get VC memory\n");
    }
}