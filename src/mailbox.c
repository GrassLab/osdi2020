#include "types.h"
#include "string.h"
#include "uart.h"
#include "mm.h"
#include "mailbox.h"

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


#define GET_BOARD_MODEL     0x00010001
#define GET_BOARD_REVISION  0x00010002
#define GET_VC_MEMORY       0x00010006

#define REQUEST_CODE        0x00000000
#define REQUEST_SUCCEED     0x80000000
#define REQUEST_FAILED      0x80000001
#define TAG_REQUEST_CODE    0x00000000
#define END_TAG             0x00000000

/**
 * Make a mailbox call. Returns 0 on failure, non-zero on success
 */
int mbox_call(uint8_t ch) {
    unsigned int r = (((unsigned int)((unsigned long)&mbox)&~0xF) | (ch&0xF));
    while(*MBOX_STATUS & MBOX_FULL);
    *MBOX_WRITE = r;
    while(true) {
        while(*MBOX_STATUS & MBOX_EMPTY);
        if(r == *MBOX_READ)
            return mbox[1]==MBOX_RESPONSE;
    }
    return 0;
}


uint32_t get_board_model() {
    mbox[0] = 7*4;                  // length of the message
    mbox[1] = REQUEST_CODE;         // this is a request message

    mbox[2] = GET_BOARD_MODEL;      // get serial number command
    mbox[3] = 4;                    // buffer size
    mbox[4] = TAG_REQUEST_CODE;
    mbox[5] = 0;                    // clear output buffer

    mbox[6] = END_TAG;

    if (mbox_call(MBOX_CH_PROP)) {
        return mbox[5];
    } else {
        return 0;
    }
}

uint32_t get_board_revision() {
    mbox[0] = 7*4;                  // length of the message
    mbox[1] = REQUEST_CODE;         // this is a request message

    mbox[2] = GET_BOARD_REVISION;   // get serial number command
    mbox[3] = 4;                    // buffer size
    mbox[4] = TAG_REQUEST_CODE;
    mbox[5] = 0;                    // clear output buffer

    mbox[6] = END_TAG;

    if (mbox_call(MBOX_CH_PROP)) {
        return mbox[5];
    } else {
        return 0;
    }
}

uint32_t get_vc_memory() {
    mbox[0] = 8*4;                  // length of the message
    mbox[1] = REQUEST_CODE;         // this is a request message

    mbox[2] = GET_VC_MEMORY;        // get serial number command
    mbox[3] = 8;                    // buffer size
    mbox[4] = TAG_REQUEST_CODE;
    mbox[5] = 0;                    // start
    mbox[6] = 0;                    // size

    mbox[7] = END_TAG;

    if (mbox_call(MBOX_CH_PROP)) {
        return mbox[5];
    } else {
        return 0;
    }
}   
