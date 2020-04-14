
#include "io.h"
#include "mbox.h"

#define GET_BOARD_REVISION  0x00010002
#define REQUEST_CODE        0x00000000
#define REQUEST_SUCCEED     0x80000000
#define REQUEST_FAILED      0x80000001
#define TAG_REQUEST_CODE    0x00000000
#define END_TAG             0x00000000
#define GET_ARM_MEMORY      0x00010005
#define GET_VC_MEMORY       0x00010006
#define GET_CLOCK           0x00030001
#define GET_FRMAEBUFFER     0x00040001
#define TAG_SETCLKRATE      0x38002
#define UART_CLOCK          0x000000002

int get_board_revision(){
    mbox[0] = 7 * 4; // buffer size in bytes
    mbox[1] = REQUEST_CODE; // tags begin
    mbox[2] = GET_BOARD_REVISION; // tag identifier
    mbox[3] = 4; // maximum of request and response value buffer's length.
    mbox[4] = TAG_REQUEST_CODE;
    mbox[5] = 0; // value buffer
    mbox[6] = END_TAG; // tags end
    return mbox_call(MBOX_CH_PROP);
}

int get_vc_memaddr(){
    mbox[0] = 8 * 4; // buffer size in bytes
    mbox[1] = REQUEST_CODE; // tags begin
    mbox[2] = GET_VC_MEMORY; // tag identifier
    mbox[3] = 8; // maximum of request and response value buffer's length.
    mbox[4] = TAG_REQUEST_CODE;
    mbox[5] = 0; // address
    mbox[6] = 0; // size
    mbox[7] = END_TAG; // tags end
    return mbox_call(MBOX_CH_PROP); 
}

int get_arm_memaddr(){
    mbox[0] = 8 * 4; // buffer size in bytes
    mbox[1] = REQUEST_CODE; // tags begin
    mbox[2] = GET_ARM_MEMORY; // tag identifier
    mbox[3] = 8; // maximum of request and response value buffer's length.
    mbox[4] = TAG_REQUEST_CODE;
    mbox[5] = 0; // address
    mbox[6] = 0; // size
    mbox[7] = END_TAG; // tags end
    return mbox_call(MBOX_CH_PROP); 
}

int get_clock(){
    mbox[0] = 8 * 4; // buffer size in bytes
    mbox[1] = REQUEST_CODE; // tags begin
    mbox[2] = GET_CLOCK; // tag identifier
    mbox[3] = 8; // maximum of request and response value buffer's length.
    mbox[4] = TAG_REQUEST_CODE;
    mbox[5] = UART_CLOCK;
    mbox[6] = 0;
    mbox[7] = END_TAG; // tags end
    return mbox_call(MBOX_CH_PROP); 
}

void get_current_el(){
    unsigned long current_el;
    __asm__ volatile("mrs %0, CurrentEL\n\t" : "=r" (current_el) : : "memory");
    printf("currentEL: %d" NEWLINE, current_el >> 2);
}
