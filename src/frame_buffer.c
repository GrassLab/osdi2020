#include "homer.h"
#include "mbox.h"
#include "peripherals/mbox.h"
#include "uart0.h"

unsigned int width, height, pitch, isrgb; /* dimensions and channel order */
unsigned char *lfb;                       /* raw frame buffer address */

void fb_init() {
    unsigned int __attribute__((aligned(16))) mbox[35];

    mbox[0] = 35 * 4;
    mbox[1] = MBOX_CODE_BUF_REQ;

    // set physical width / height
    mbox[2] = MBOX_TAG_SET_PHY_WIDTH_HEIGHT;
    mbox[3] = 8;
    mbox[4] = 8;
    mbox[5] = 1024;  //FrameBufferInfo.width
    mbox[6] = 768;   //FrameBufferInfo.height

    // set virtual width / height
    mbox[7] = MBOX_TAG_SET_VTL_WIDTH_HEIGHT;
    mbox[8] = 8;
    mbox[9] = 8;
    mbox[10] = 1024;  //FrameBufferInfo.virtual_width
    mbox[11] = 768;   //FrameBufferInfo.virtual_height

    // set virtual offset
    mbox[12] = MBOX_TAG_SET_VTL_OFFSET;  
    mbox[13] = 8;
    mbox[14] = 8;
    mbox[15] = 0;  //FrameBufferInfo.x_offset
    mbox[16] = 0;  //FrameBufferInfo.y.offset

    // set depth
    mbox[17] = MBOX_TAG_SET_DEPTH;
    mbox[18] = 4;
    mbox[19] = 4;
    mbox[20] = 32;  //FrameBufferInfo.depth

    // set pixel order
    mbox[21] = MBOX_TAG_SET_PIXEL_ORDER;
    mbox[22] = 4;
    mbox[23] = 4;
    mbox[24] = 1;  //RGB, not BGR preferably

    // get framebuffer, gets alignment on request
    mbox[25] = MBOX_TAG_ALLOCATE_BUFFER;  
    mbox[26] = 8;
    mbox[27] = 8;
    mbox[28] = 4096;  //FrameBufferInfo.pointer
    mbox[29] = 0;     //FrameBufferInfo.size

    // get pitch
    mbox[30] = MBOX_TAG_GET_PITCH;  
    mbox[31] = 4;
    mbox[32] = 4;
    mbox[33] = 0;  //FrameBufferInfo.pitch

    mbox[34] = 0x0;

    if (mbox_call(mbox, 8) && mbox[20] == 32 && mbox[28] != 0) {
        mbox[28] &= 0x3FFFFFFF;  //convert GPU address to ARM address
        width = mbox[5];         //get actual physical width
        height = mbox[6];        //get actual physical height
        pitch = mbox[33];        //get number of bytes per line
        isrgb = mbox[24];        //get the actual channel order
        lfb = (void *)((unsigned long)mbox[28]);
    }
    else {
        uart_printf("Unable to set screen resolution to 1024x768x32\n");
    }
}

void fb_showpicture() {
    int x, y;
    unsigned char *ptr = lfb;
    char *data = homer_data, pixel[4];

    ptr += (height - homer_height) / 2 * pitch + (width - homer_width) * 2;
    for (y = 0; y < homer_height; y++) {
        for (x = 0; x < homer_width; x++) {
            HEADER_PIXEL(data, pixel);
            // the image is in RGB. So if we have an RGB framebuffer, we can copy the pixels
            // directly, but for BGR we must swap R (pixel[0]) and B (pixel[2]) channels.
            *((unsigned int *)ptr) = isrgb ? *((unsigned int *)&pixel) : (unsigned int)(pixel[0] << 16 | pixel[1] << 8 | pixel[2]);
            ptr += 4;
        }
        ptr += pitch - homer_width * 4;
    }
}