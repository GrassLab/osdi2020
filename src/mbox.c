#include "peripherals/mbox.h"

#include "uart0.h"

uint64_t arm_memory_start, arm_memory_end;
uint64_t vc_memory_start, vc_memory_end;

int mbox_call(unsigned int* mbox, unsigned char channel) {
    unsigned int r = (unsigned int)(((unsigned long)mbox) & (~0xF)) | (channel & 0xF);
    // wait until full flag unset
    while (*MBOX_STATUS & MBOX_FULL) {
    }
    // write address of message + channel to mailbox
    *MBOX_WRITE = r;
    // wait until response
    while (1) {
        // wait until empty flag unset
        while (*MBOX_STATUS & MBOX_EMPTY) {
        }
        // is it a response to our msg?
        if (r == *MBOX_READ) {
            // check is response success
            return mbox[1] == MBOX_CODE_BUF_RES_SUCC;
        }
    }
    return 0;
}

void mbox_board_revision() {
    unsigned int __attribute__((aligned(16))) mbox[7];
    mbox[0] = 7 * 4;  // buffer size in bytes
    mbox[1] = MBOX_CODE_BUF_REQ;
    // tags begin
    mbox[2] = MBOX_TAG_GET_BOARD_REVISION;  // tag identifier
    mbox[3] = 4;                            // maximum of request and response value buffer's length.
    mbox[4] = MBOX_CODE_TAG_REQ;            // tag code
    mbox[5] = 0;                            // value buffer
    mbox[6] = 0x0;                          // end tag
    // tags end
    mbox_call(mbox, 8);
    uart_printf("Board Revision: %x\n", mbox[5]);
}

void mbox_vc_memory() {
    unsigned int __attribute__((aligned(16))) mbox[8];
    mbox[0] = 8 * 4;  // buffer size in bytes
    mbox[1] = MBOX_CODE_BUF_REQ;
    // tags begin
    mbox[2] = MBOX_TAG_GET_VC_MEMORY;  // tag identifier
    mbox[3] = 8;                       // maximum of request and response value buffer's length.
    mbox[4] = MBOX_CODE_TAG_REQ;       // tag code
    mbox[5] = 0;                       // base address
    mbox[6] = 0;                       // size in bytes
    mbox[7] = 0x0;                     // end tag
    // tags end
    mbox_call(mbox, 8);
    vc_memory_start = mbox[5];
    vc_memory_end = mbox[5] + mbox[6];
}

void mbox_arm_memory() {
    unsigned int __attribute__((aligned(16))) mbox[8];
    mbox[0] = 8 * 4;  // buffer size in bytes
    mbox[1] = MBOX_CODE_BUF_REQ;
    // tags begin
    mbox[2] = MBOX_TAG_GET_ARM_MEMORY;  // tag identifier
    mbox[3] = 8;                        // maximum of request and response value buffer's length.
    mbox[4] = MBOX_CODE_TAG_REQ;        // tag code
    mbox[5] = 0;                        // base address
    mbox[6] = 0;                        // size in bytes
    mbox[7] = 0x0;                      // end tag
    // tags end
    mbox_call(mbox, 8);
    arm_memory_start = mbox[5];
    arm_memory_end = mbox[5] + mbox[6];
}
