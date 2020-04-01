#include "types.h"
#include "string.h"
#include "uart.h"
#include "mm.h"
#include "mailbox.h"

/**
 * Make a mailbox call. Returns 0 on failure, non-zero on success
 */
int mbox_call(volatile unsigned int *message, uint8_t channel) {
    unsigned int r = (((unsigned int)((unsigned long)message)&~0xF) | (channel&0xF));
    while(mm_read(MBOX_STATUS) & MBOX_FULL);
    mm_write(MBOX_WRITE, r);
    while(true) {
        while(mm_read(MBOX_STATUS) & MBOX_EMPTY);
        if(r == mm_read(MBOX_READ))
            return message[1]==MBOX_RESPONSE;
    }
    return 0;
}

uint32_t get_board_revision() {

    volatile unsigned int __attribute__((aligned(16))) mbox[7] = {
        7*4, REQUEST_CODE, GET_BOARD_REVISION, 4, TAG_REQUEST_CODE, 0, END_TAG
    };

    if (mbox_call(mbox, MBOX_CH_PROP)) {
        return mbox[5];
    } else {
        uart_log(LOG_ERROR, "Unable to get the board revision");
        return 0;
    }
}

uint32_t get_vc_memory() {
    
   volatile unsigned int __attribute__((aligned(16))) mbox[8] = {
        8*4, REQUEST_CODE, GET_VC_MEMORY, 8, TAG_REQUEST_CODE, 0, 0, END_TAG
    };

    if (mbox_call(mbox, MBOX_CH_PROP)) {
        return mbox[5];
    } else {
        uart_log(LOG_ERROR, "Unable to get the video memory map");
        return 0;
    }
}   
