#include "uart.h"
#include "mailbox.h"
//#include "teacher.h"
//#include "splash.h"
unsigned int width, height, pitch, isrgb;   /* dimensions and channel order */
unsigned char *lfb;                         /* raw frame buffer address */

/**
 * Set screen resolution to 1024x768
 */
void lfb_init()
{
    _mbox[0] = 35*4;
    _mbox[1] = MBOX_REQUEST;

    _mbox[2] = 0x48003;  //set phy wh
    _mbox[3] = 8;
    _mbox[4] = 8;
    _mbox[5] = 1024;         //FrameBufferInfo.width
    _mbox[6] = 768;          //FrameBufferInfo.height

    _mbox[7] = 0x48004;  //set virt wh
    _mbox[8] = 8;
    _mbox[9] = 8;
    _mbox[10] = 1024;        //FrameBufferInfo.virtual_width
    _mbox[11] = 768;         //FrameBufferInfo.virtual_height

    _mbox[12] = 0x48009; //set virt offset
    _mbox[13] = 8;
    _mbox[14] = 8;
    _mbox[15] = 0;           //FrameBufferInfo.x_offset
    _mbox[16] = 0;           //FrameBufferInfo.y.offset

    _mbox[17] = 0x48005; //set depth
    _mbox[18] = 4;
    _mbox[19] = 4;
    _mbox[20] = 32;          //FrameBufferInfo.depth

    _mbox[21] = 0x48006; //set pixel order
    _mbox[22] = 4;
    _mbox[23] = 4;
    _mbox[24] = 1;           //RGB, not BGR preferably

    _mbox[25] = 0x40001; //get framebuffer, gets alignment on request
    _mbox[26] = 8;
    _mbox[27] = 8;
    _mbox[28] = 4096;        //FrameBufferInfo.pointer
    _mbox[29] = 0;           //FrameBufferInfo.size

    _mbox[30] = 0x40008; //get pitch
    _mbox[31] = 4;
    _mbox[32] = 4;
    _mbox[33] = 0;           //FrameBufferInfo.pitch

    _mbox[34] = MBOX_TAG_LAST;

    //this might not return exactly what we asked for, could be
    //the closest supported resolution instead
    if(mbox_call(MBOX_CH_PROP) && _mbox[20]==32 && _mbox[28]!=0) {
        _mbox[28]&=0x3FFFFFFF;   //convert GPU address to ARM address
        width=_mbox[5];          //get actual physical width
        height=_mbox[6];         //get actual physical height
        pitch=_mbox[33];         //get number of bytes per line
        isrgb=_mbox[24];         //get the actual channel order
        lfb=(void*)((unsigned long)_mbox[28]);
    } else {
        uart_puts("Unable to set screen resolution to 1024x768x32\n");
    }
}

/**
 * Show a picture
 */
void lfb_showpicture()
{
    int x,y;
    unsigned char *ptr=lfb;
    //char *data=homer_data;
    char pixel[4] = {255, 156, 156, 255}, bpixel[4] = {0,0,0,0};

    /*ptr += (height-homer_height)/2*pitch + (width-homer_width)*2;
    for(y=0;y<homer_height;y++) {
        for(x=0;x<homer_width;x++) {
            HEADER_PIXEL(data, pixel);
            // the image is in RGB. So if we have an RGB framebuffer, we can copy the pixels
            // directly, but for BGR we must swap R (pixel[0]) and B (pixel[2]) channels.
            *((unsigned int*)ptr) = isrgb ? *((unsigned int *)&pixel) : (unsigned int)(pixel[0]<<16 | pixel[1]<<8 | pixel[2]);
            ptr+=4;
        }
        ptr+=pitch-homer_width*4;
    }*/
    
    for(y=0;y<1024;y++) {
        for(x=0;x<768;x++) {
            if( ((x/32)%2==0 && (y/24)%2==0) || ((x/32)%2==1 && (y/24)%2==1))
                *((unsigned int*)ptr) = isrgb ? *((unsigned int *)&pixel) : (unsigned int)(pixel[0]<<16 | pixel[1]<<8 | pixel[2]);
            else
                *((unsigned int*)ptr) = isrgb ? *((unsigned int *)&bpixel) : (unsigned int)(bpixel[0]<<16 | bpixel[1]<<8 | bpixel[2]);
            ptr+=4;
        }
    }
}