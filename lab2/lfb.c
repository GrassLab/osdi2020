#include "uart.h"
#include "mbox.h"

#define RGB_WHITE 	0xFFFFFF
#define RGB_BLACK 	0x000000

unsigned int width, height, pitch, isrgb;   /* dimensions and channel order */
unsigned char *lfb;                         /* raw frame buffer address */

void lfb_init(){
	set_lfb_init_mbox();

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

void lfb_showpicture(){
	int pic_width = 600;
	int pic_height = 600;

	unsigned char *ptr = lfb;

	ptr += (height - pic_height)/2*pitch + (width - pic_width)*2;

	int splash_block = 30;
	for(int y=0; y<pic_height/splash_block; y++){
		for(int y_inblock=0; y_inblock<splash_block; y_inblock++){
			for(int x=0; x<pic_width/splash_block; x++){

				unsigned int color;
				if(y%2==0){
					if(x%2==0) color = RGB_WHITE;
					else color = RGB_BLACK;
				}else{
					if(x%2==0) color = RGB_BLACK;
					else color = RGB_WHITE;
				}

				for(int x_inblock=0; x_inblock<splash_block; x_inblock++){
					*((unsigned int *)ptr) = color;	
					ptr += 4; // 4 char to express a pixel
				}
			}
			ptr += pitch - pic_width*4;
		}

		
	}
}