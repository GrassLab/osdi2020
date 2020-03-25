#include "types.h"
#include "string.h"
#include "uart.h"
#include "mm.h"
#include "mailbox.h"

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
int mbox_call(volatile uint32_t *message, uint8_t channel) {
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

    volatile unsigned int  __attribute__((aligned(16))) mbox[7] = {
        7*4, REQUEST_CODE, GET_BOARD_REVISION, 4, TAG_REQUEST_CODE, 0, END_TAG
    };

    if (mbox_call(mbox, MBOX_CH_PROP)) {
        return mbox[5];
    } else {
        return 0;
    }
}

uint32_t get_vc_memory() {
    
   volatile unsigned int  __attribute__((aligned(16))) mbox[8] = {
        8*4, REQUEST_CODE, GET_VC_MEMORY, 8, TAG_REQUEST_CODE, 0, 0, END_TAG
    };

    if (mbox_call(mbox, MBOX_CH_PROP)) {
        return mbox[5];
    } else {
        return 0;
    }
}   
