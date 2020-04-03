#include "uart.h"
#include "mbox.h"

void get_board_revision(){
    mbox[0] = 7 * 4; // buffer size in bytes
    mbox[1] = MBOX_REQUEST;
    // tags begin
    mbox[2] = GET_BOARD_REVISION; // tag identifier
    mbox[3] = 4; // maximum of request and response value buffer's length.
    mbox[4] = TAG_REQUEST_CODE;
    mbox[5] = 0; // value buffer
    // tags end
    mbox[6] = MBOX_TAG_LAST;

    if(mbox_call(MBOX_CH_PROP)){
        uart_puts("Board revision: ");
        uart_hex(mbox[5]); // it should be 0xa020d3 for rpi3 b+
        uart_puts("\n");
    }else
        uart_puts("Unable to query \"board revision\"!\n");
}

void get_VC_base_addr(){
    mbox[0] = 8 * 4; // buffer size in bytes
    mbox[1] = MBOX_REQUEST;
    // tags begin
    mbox[2] = GET_VC_MEMORY; // tag identifier
    mbox[3] = 8; // maximum of request and response value buffer's length.
    mbox[4] = TAG_REQUEST_CODE;
    mbox[5] = 0; // base addr value buffer
    mbox[6] = 0; // mem size value buffer  
    // tags end
    mbox[7] = MBOX_TAG_LAST;

    if(mbox_call(MBOX_CH_PROP)){
        uart_puts("VC Core base address: ");
        uart_hex(mbox[5]);
        uart_puts(" size: ");
        uart_hex(mbox[6]);
        uart_puts("\n");
    }else
        uart_puts("Unable to query \"VC base address\"!\n");
}

void get_clock_rate(unsigned int clock_id){
    mbox[0] = 7 * 4;
    mbox[1] = MBOX_REQUEST;
    mbox[2] = MBOX_TAG_GETCLKRATE; // set clock rate
    mbox[3] = 8;
    mbox[4] = TAG_REQUEST_CODE;
    mbox[5] = clock_id;           // UART clock
    mbox[6] = 0;       
    mbox[7] = MBOX_TAG_LAST;
    mbox_call(MBOX_CH_PROP);
    uart_puts("PL011 UART clock rate: ");
    uart_send_int(mbox[6]);
    uart_puts("\n");
}