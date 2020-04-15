#include "uart.h"
#include "mbox.h"

#define GET_BOARD_REVISION  0x00010002
#define GET_VC_MEMORY       0x00010006
#define REQUEST_CODE        0x00000000
#define REQUEST_SUCCEED     0x80000000
#define REQUEST_FAILED      0x80000001
#define TAG_REQUEST_CODE    0x00000000
#define END_TAG             0x00000000

void show_board_revision() {
    mbox[0] = 7 * sizeof(int); // buffer size in bytes
    mbox[1] = REQUEST_CODE;
    mbox[2] = GET_BOARD_REVISION; // tag identifier
    mbox[3] = 4; // maximum of request and response value buffer's length.
    mbox[4] = TAG_REQUEST_CODE;
    mbox[5] = 0; // value buffer
    mbox[6] = END_TAG; // tags end
    if (mbox_call(MBOX_CH_PROP)) {
        uart_puts("Board revision: ");
        uart_hex(mbox[5]);
        uart_puts("\n");
    } else {
        uart_puts("Board revision: N/A\n");
    }
}

void show_vc_memory() {
    mbox[0] = 8 * sizeof(int);
    mbox[1] = REQUEST_CODE;
    mbox[2] = GET_VC_MEMORY;
    mbox[3] = 8;
    mbox[4] = TAG_REQUEST_CODE;
    mbox[5] = 0;
    mbox[6] = 0;
    mbox[7] = END_TAG;
    if (mbox_call(MBOX_CH_PROP)) {
        uart_puts("Base address of videoCore memory: ");
        uart_hex(mbox[5]);
        uart_puts("\n");
        uart_puts("Size of videoCore meory: ");
        uart_hex(mbox[6]);
        uart_puts("\n");
    } else {
        uart_puts("Base address of videoCore memory: N/A\n");
        uart_puts("Size of videoCore meory: N/A\n");
    }
}
