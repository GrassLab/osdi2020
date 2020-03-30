#include "framebuffer.h"
#include "mailbox.h"
unsigned int width, height, pitch, isrgb;

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
    uart_puts("done\n");
    return;
}

unsigned long framebuffer_init()
{
    //set the screen size
    mbox[0] = 35 * 4; //align 16 bytes
    mbox[1] = MBOX_REQUEST;

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
    mbox[14] = MBOX_REQUEST;
    mbox[15] = 0;           //FrameBufferInfo.x_offset
    mbox[16] = 0;           //FrameBufferInfo.y.offset

    mbox[17] = SET_DEPTH;
    mbox[18] = 4;
    mbox[19] = TAG_REQUEST_CODE;
    mbox[20] = BITS_PER_PIXEL; // RGB 8 bit 8 bit 8 bit

    mbox[21] = SET_PIXEL_ORDER; //set pixel order
    mbox[22] = 4;
    mbox[23] = MBOX_REQUEST ;
    mbox[24] = 1;           //RGB, not BGR preferably
    
    mbox[25] = ALLOCATE_BUFFER;
    mbox[26] = 8;
    mbox[27] = MBOX_REQUEST;
    mbox[28] = 4096;
    mbox[29] = 0;

    mbox[30] = 0x40008;
    mbox[31] = 4;
    mbox[32] = MBOX_REQUEST;
    mbox[33] = 0;
    mbox[34] = 0; //TAG END
    uart_puts("frame setting\r\n");

    if(mbox_call(MBOX_CH_PROP) && mbox[20]==BITS_PER_PIXEL && mbox[28]!=0) {
        mbox[28]&=0x3FFFFFFF;   //convert GPU address to ARM address
        width=mbox[5];          //get actual physical width
        height=mbox[6];         //get actual physical height
        pitch=mbox[33];         //get number of bytes per line
        isrgb=mbox[24];         //get the actual channel order
    } else {
        uart_puts("Unable to set screen resolution to 680 * 480 * 24\n");
    }
    return (unsigned long)mbox[28];
}