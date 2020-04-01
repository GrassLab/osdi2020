#include "homer.h"
#include "mailbox.h"
#include "uart.h"


unsigned int width, height, pitch, isrgb;   /* dimensions and channel order */
unsigned char *fbuff;                       /* raw frame buffer address */

/**
 * Set screen resolution to 1024x768
 */
void fbuff_init()
{
    mbox[0] = 35*4;
    mbox[1] = MBOX_REQUEST_CODE;

    mbox[2] = MBOX_TAG_SET_PHY_DISP;
    mbox[3] = 2*4;
    mbox[4] = MBOX_TAG_REQUEST_CODE;
    mbox[5] = 1024;
    mbox[6] = 768;

    mbox[7] = MBOX_TAG_SET_VBUFF;
    mbox[8] = 2*4;
    mbox[9] = MBOX_TAG_REQUEST_CODE;
    mbox[10] = 1024;
    mbox[11] = 768;

    mbox[12] = MBOX_TAG_SET_VOFFSET;
    mbox[13] = 2*4;
    mbox[14] = MBOX_TAG_REQUEST_CODE;
    mbox[15] = 0;
    mbox[16] = 0;

    mbox[17] = MBOX_TAG_SET_DEPTH;
    mbox[18] = 1*4;
    mbox[19] = MBOX_TAG_REQUEST_CODE;
    mbox[20] = 32;

    mbox[21] = MBOX_TAG_SET_PIXEL_ORD;
    mbox[22] = 1*4;
    mbox[23] = MBOX_TAG_REQUEST_CODE;
    mbox[24] = 1;//RGB

    mbox[25] = MBOX_TAG_ALLOCATE_BUFF;
    mbox[26] = 2*4;
    mbox[27] = MBOX_TAG_REQUEST_CODE;
    mbox[28] = 4096;//base
    mbox[29] = 0;//size

    mbox[30] = MBOX_TAG_GET_PITCH;
    mbox[31] = 1*4;
    mbox[32] = MBOX_TAG_REQUEST_CODE;
    mbox[33] = 0;

    mbox[34] = MBOX_END_TAG;

    //this might not return exactly what we asked for, could be
    //the closest supported resolution instead
    if(mbox_call(MBOX_CH_PROPT_ARM_VC) && mbox[20]==32 && mbox[28]!=0) {
        mbox[28]&=0x3FFFFFFF;   //convert GPU address to ARM address
        width=mbox[5];          //get actual physical width
        height=mbox[6];         //get actual physical height
        pitch=mbox[33];         //get number of bytes per line
        isrgb=mbox[24];         //get the actual channel order
        fbuff=(void*)((unsigned long)mbox[28]);
    } else {
        uart_puts("Unable to set screen resolution to 1024x768x32\n");
    }
}

/**
 * Show a picture
 */
void fbuff_showBitmap()
{
    int x,y;
    unsigned char *ptr=fbuff;
    char *data=homer_data, pixel[4];

    ptr += (height-homer_height)/2*pitch + (width-homer_width)*2;
    for(y=0;y<homer_height;y++) {
        for(x=0;x<homer_width;x++) {
            HEADER_PIXEL(data, pixel);
            // the image is in RGB. So if we have an RGB framebuffer, we can copy the pixels
            // directly, but for BGR we must swap R (pixel[0]) and B (pixel[2]) channels.
            *((unsigned int*)ptr)=isrgb ? *((unsigned int *)&pixel) : (unsigned int)(pixel[0]<<16 | pixel[1]<<8 | pixel[2]);
            ptr+=4;
        }
        ptr+=pitch-homer_width*4;
    }
}