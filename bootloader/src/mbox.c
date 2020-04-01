#include "peripherals/mbox.h"

#include "uart0.h"

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
