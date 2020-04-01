#include "tools.h"
#include "uart.h"
#include "mbox.h"
#include "get_information.h"
#include "gpio.h"
#include "load_kernel.h"
#include "utils.h"

unsigned int width, height, pitch, isrgb;   /* dimensions and channel order */
unsigned char *lfb;                         /* raw frame buffer address */

/**
 * Set screen resolution to 1024x768
 */
void lfb_init()
{
    mbox[0] = 35*4;
    mbox[1] = MBOX_REQUEST;

    mbox[2] = 0x48003;  //set phy wh
    mbox[3] = 8;
    mbox[4] = 8;
    mbox[5] = 1024;         //FrameBufferInfo.width
    mbox[6] = 768;          //FrameBufferInfo.height

    mbox[7] = 0x48004;  //set virt wh
    mbox[8] = 8;
    mbox[9] = 8;
    mbox[10] = 1024;        //FrameBufferInfo.virtual_width
    mbox[11] = 768;         //FrameBufferInfo.virtual_height

    mbox[12] = 0x48009; //set virt offset
    mbox[13] = 8;
    mbox[14] = 8;
    mbox[15] = 0;           //FrameBufferInfo.x_offset
    mbox[16] = 0;           //FrameBufferInfo.y.offset

    mbox[17] = 0x48005; //set depth
    mbox[18] = 4;
    mbox[19] = 4;
    mbox[20] = 32;          //FrameBufferInfo.depth

    mbox[21] = 0x48006; //set pixel order
    mbox[22] = 4;
    mbox[23] = 4;
    mbox[24] = 1;           //RGB, not BGR preferably

    mbox[25] = 0x40001; //get framebuffer, gets alignment on request
    mbox[26] = 8;
    mbox[27] = 8;
    mbox[28] = 4096;        //FrameBufferInfo.pointer
    mbox[29] = 0;           //FrameBufferInfo.size

    mbox[30] = 0x40008; //get pitch
    mbox[31] = 4;
    mbox[32] = 4;
    mbox[33] = 0;           //FrameBufferInfo.pitch

    mbox[34] = MBOX_TAG_LAST;

    //this might not return exactly what we asked for, could be
    //the closest supported resolution instead
    if(mbox_call(MBOX_CH_PROP) && mbox[20]==32 && mbox[28]!=0) {
        mbox[28]&=0x3FFFFFFF;   //convert GPU address to ARM address
        width=mbox[5];          //get actual physical width
        height=mbox[6];         //get actual physical height
        pitch=mbox[33];         //get number of bytes per line
        isrgb=mbox[24];         //get the actual channel order
        lfb=(void*)((unsigned long)mbox[28]);
    } else {
        uart_puts("Unable to set screen resolution to 1024x768x32\n");
    }
}

/**
 * Show a picture
 */
void lfb_showpicture()
{
	int x, y;
	unsigned char *ptr = lfb;
	char pixel[4]; // rgb
	uart_puts("Start showing framebuffer\n");
	_Bool y_black = 0;
	_Bool x_black = 0;
	
	for(int y=0;y<height;y++){
		y_black=(y/32)&1;
		if(y_black){
			pixel[0] = 0; pixel[1] = 0; pixel[2] = 0;
		}else{
			pixel[0] = 255; pixel[1] = 255; pixel[2] = 255;
		}
    	for(int x=0;x<width;x++){
      		x_black= ((x/32)&1) ^ y_black;
			if(x_black){
				pixel[0] = 0; pixel[1] = 0; pixel[2] = 0;
			}else{
				pixel[0] = 255; pixel[1] = 255; pixel[2] = 255;
			}
      		*((unsigned int*) ptr) = *((unsigned int *)&pixel);
      		ptr = ptr + 4;
    	}
    	ptr = ptr + pitch - (width * 4);
  	}
}
