#include "gpio.h"
#include "uart.h"
#include "mbox.h"
unsigned int width, height, pitch, isrgb;   /* dimensions and channel order */
int mbox_call(unsigned char ch);

void write_buf(unsigned long loc) {
    int x,y;
    int value = 255;
    unsigned char *location = (unsigned char *)loc;
    for (y = 0 ; y < FB_HEIGHT ; y++) {
        for (x = 0 ; x < FB_WIDTH ; x++) {
            location[0] = value;
            location[1] = value;
            location[2] = value;
            location[3] = value;
            if ((x%20) == 0) {
                value = (value == 255 ? 0 : 255);
            }
            location += 4;
        }
        if ((y%20) == 0) {
            value = (value == 255 ? 0 : 255);
        }
    }
    uart_send_string("done\n");
    return;
}

unsigned long framebuffer_init()
{
    //set the screen size
    mbox[0] = 35 * 4; //align 16 bytes
    mbox[1] = REQUEST_CODE;

    mbox[2] = SET_PHYSICAL_DISPLAY;
    mbox[3] = 8;
    mbox[4] = TAG_REQUEST_CODE;
    mbox[5] = FB_WIDTH;
    mbox[6] = FB_HEIGHT;

    mbox[7] = SET_VIRTUAL_DISPLAY;
    mbox[8] = 8;
    mbox[9] = TAG_REQUEST_CODE;
    mbox[10] = FB_WIDTH;
    mbox[11] = FB_HEIGHT;
    
    mbox[12] = VIRTUAL_OFFSET; //set virt offset
    mbox[13] = 8;
    mbox[14] = REQUEST_CODE;
    mbox[15] = 0;           //FrameBufferInfo.x_offset
    mbox[16] = 0;           //FrameBufferInfo.y.offset

    mbox[17] = SET_DEPTH;
    mbox[18] = 4;
    mbox[19] = TAG_REQUEST_CODE;
    mbox[20] = BITS_PER_PIXEL; // RGB 8 bit 8 bit 8 bit

    mbox[21] = SET_PIXEL_ORDER; //set pixel order
    mbox[22] = 4;
    mbox[23] = REQUEST_CODE ;
    mbox[24] = 1;           //RGB, not BGR preferably
    
    mbox[25] = ALLOCATE_BUFFER;
    mbox[26] = 8;
    mbox[27] = REQUEST_CODE;
    mbox[28] = 4096;
    mbox[29] = 0;

    mbox[30] = 0x40008;
    mbox[31] = 4;
    mbox[32] = REQUEST_CODE;
    mbox[33] = 0;
    mbox[34] = 0; //TAG END
    uart_send_string("frame setting\r\ne");

    if(mbox_call(MBOX_CH_PROP) && mbox[20]==BITS_PER_PIXEL && mbox[28]!=0) {
        mbox[28]&=0x3FFFFFFF;   //convert GPU address to ARM address
        width=mbox[5];          //get actual physical width
        height=mbox[6];         //get actual physical height
        pitch=mbox[33];         //get number of bytes per line
        isrgb=mbox[24];         //get the actual channel order
    } else {
        uart_send_string("Unable to set screen resolution to 680 * 480 * 24\n");
    }
    return (unsigned long)mbox[28];
}

void get_vc_base_address()
{
    mbox[0] = 8 * 4;
    mbox[1] = REQUEST_CODE;
    mbox[2] = GET_VC_MEMORY;
    mbox[3] = 8;
    mbox[4] = TAG_REQUEST_CODE;
    mbox[5] = 0; // value buffer
    mbox[6] = 0; // value buffer
    mbox[7] = END_TAG;
    mbox_call(8);
    uart_send_string("VC base address : ");
    uart_send_hex(mbox[5]);
    uart_send_string("VC size : ");
    uart_send_hex(mbox[6]);
}

void get_board_revision()
{
    mbox[0] = 7 * 4; // buffer size in bytes
    mbox[1] = REQUEST_CODE;
    // tags begin
    mbox[2] = GET_BOARD_REVISION; // tag identifier
    mbox[3] = 4; // maximum of request and response value buffer's length.
    mbox[4] = TAG_REQUEST_CODE;
    mbox[5] = 0; // value buffer
    // tags end
    mbox[6] = END_TAG;

    mbox_call(8); // message passing procedure call, you should implement it following the 6 steps provided above.
    uart_send_string("board revision : ");
    uart_send_hex(mbox[5]);
}


/**
 * Make a mailbox call. Returns 0 on failure, non-zero on success
 */
int mbox_call(unsigned char ch)
{
    unsigned int r = (((unsigned int)((unsigned long)&mbox)&~0xF) | (ch&0xF)); //get the address of mbox buffer and ch
    /* wait until we can write to the mailbox */
    do{asm volatile("nop");}while(*MBOX_STATUS & MBOX_FULL);
    /* write the address of our message to the mailbox with channel identifier */
    *MBOX_WRITE = r;
    /* now wait for the response */
    while(1) {
        /* is there a response? */
        do{asm volatile("nop");}while(*MBOX_STATUS & MBOX_EMPTY);
        /* is it a response to our message? */
        if(r == *MBOX_READ)
            /* is it a valid successful response? */
            return mbox[1]==MBOX_RESPONSE;
    }
    return 0;
}