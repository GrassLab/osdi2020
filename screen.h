/*****

Document:
https://github.com/raspberrypi/firmware/wiki/Mailbox-property-interface

*****/
#ifndef _SCREEN_H_ 
#define _SCREEN_H_
#include "uart.h"
#include "mailbox.h"

#define GET_BOARD_REVISION  0x00010002
#define GET_VC_MEMORY       0x00010006
#define REQUEST_CODE        0x00000000
#define REQUEST_SUCCEED     0x80000000
#define REQUEST_FAILED      0x80000001
#define TAG_REQUEST_CODE    0x00000000
#define END_TAG             0x00000000

void get_board_revision(){
    mailbox[0] = 7*4;                  // length of the message
    mailbox[1] = REQUEST_CODE;         // this is a request message
    
    mailbox[2] = GET_BOARD_REVISION;   // get serial number command
    mailbox[3] = 4;                    // buffer size
    mailbox[4] = TAG_REQUEST_CODE;
    mailbox[5] = 0;                    // clear output buffer

    mailbox[6] = END_TAG;

    // send the message to the GPU and receive answer
    if (mailbox_call(MBOX_CH_PROP)) {
        uart_puts("Board Revision:\t\t");

        // it should be 0xa020d3 for rpi3 B+
        // in qemu, show 0xa02082 for rpi3 B Revision:1.2 RAM:1GB Sony UK
        // https://www.raspberrypi.org/documentation/hardware/raspberrypi/revision-codes/README.md
        uart_send_hex(mailbox[5]);
        uart_puts("\n");
    } else {
        uart_puts("MailBox fail\n");
    }
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

    if(mailbox_call(MBOX_CH_PROP)){
        uart_puts("VC Core base address:\t");
        uart_send_hex(mailbox[5]);
        uart_puts("\n");
        uart_puts("VC memory size:\t\t");
        uart_send_hex(mailbox[6]);
        uart_puts("\n");
    }

}



#endif
