#include "include/uart.h"
#include "include/mbox.h"

unsigned int width,height,pitch,isrgb;
unsigned char *fb;

void fb_init(){
	//Set physical display
	mbox[0] = 8*4;
	mbox[1] = REQUEST_CODE;
	mbox[2] = SET_PHY_DISPLAY;
	mbox[3] = 8; // response
	mbox[4] = 8; // request
	mbox[5] = 1024;
	mbox[6] = 768;
	mbox[7] = END_TAG;
	mbox_call(8);

	unsigned tmp_width = mbox[5];
	unsigned tmp_height = mbox[6];

        //Set virtual display
	mbox[0] = 8*4;
	mbox[1] = REQUEST_CODE;
	mbox[2] = SET_VIR_DISPLAY;
	mbox[3] = 8;
	mbox[4] = 8;
	mbox[5] = 1024;
	mbox[6] = 768;
	mbox[7] = END_TAG;
	mbox_call(8);

        //Set virtual offset
	mbox[0] = 8*4;
	mbox[1] = REQUEST_CODE;
	mbox[2] = SET_VIR_OFFSET;
	mbox[3] = 8;
	mbox[4] = 0;
	mbox[5] = 0;
	mbox[6] = 0;
	mbox[7] = END_TAG;
	mbox_call(8);

        //Set depth
	mbox[0] = 7*4;
	mbox[1] = REQUEST_CODE;
	mbox[2] = SET_DEPTH;
	mbox[3] = 4;
	mbox[4] = 0;
	mbox[5] = 32;
	mbox[6] = END_TAG;
	mbox_call(8);
	
	unsigned int depth = mbox[5];

	//Set Pixel Order
	mbox[0] = 7*4;
	mbox[1] = REQUEST_CODE;
	mbox[2] = SET_PIXEL_ORDER;
	mbox[3] = 4;
	mbox[4] = 0;
	mbox[5] = 1;
	mbox[6] = END_TAG;
	mbox_call(8);
	
	unsigned int tmp_isrgb = mbox[5];
	
	//Allocate buffer
	mbox[0] = 8*4;
	mbox[1] = REQUEST_CODE;
	mbox[2] = ALLOCATE_BUFFER;
	mbox[3] = 8;
	mbox[4] = 4;
	mbox[5] = 4096;
	mbox[6] = 0;
	mbox[7] = END_TAG;
	mbox_call(8);
        
	unsigned int pointer = mbox[5];

	//Get pitch
	mbox[0] = 7*4;
	mbox[1] = REQUEST_CODE;
	mbox[2] = GET_PITCH;
	mbox[3] = 4;
	mbox[4] = 0;
	mbox[5] = 0;
	mbox[6] = END_TAG;
	mbox_call(8);
	
	unsigned int tmp_pitch = mbox[5];

	if(depth==32&&pointer!=0){
		pointer &= 0x3FFFFFFF;
		width = tmp_width;
		height = tmp_height;
		pitch = tmp_pitch;
		isrgb = tmp_isrgb;
		/* check if setting  are right
		uart_send_string("Width:");
		uart_hex(width);
		uart_send_string("\r\nHeight:");
		uart_hex(height);
		uart_send_string("\r\nPitch:");
		uart_hex(pitch);
		uart_send_string("\r\nIsRGB:");
		uart_hex(isrgb);
		*/		
		fb = (void*)((unsigned long)pointer);
		uart_send_string("\r\nSetting frame buffer sucess!\r\n");
	}
	else{
	 uart_send_string("Unable to set frame buffer:(\r\n");
	}
}


void fb_show()
{
    int x,y;
    unsigned char *ptr=fb; 
    char pixel[4];
   
    for(y=0;y<height;y++) {
        for(x=0;x<width;x++) {
            if(y>200&&y<500){	
	    	pixel[0] = 255;
	    	pixel[1] = 255;
	    	pixel[2] = 255;
		}
	    else{
	    	pixel[0] = 0;
		pixel[1] = 0;
		pixel[2] = 0;
	    }
	    
            // the image is in RGB. So if we have an RGB framebuffer, we can copy the pixels
            // directly, but for BGR we must swap R (pixel[0]) and B (pixel[2]) channels.
            *((unsigned int*)ptr) = isrgb ? *((unsigned int *)&pixel) : (unsigned int)(pixel[0]<<16 | pixel[1]<<8 | pixel[2]);
            
	    //*((unsigned int*)ptr) = isrgb
	    ptr+=4;
        }
        ptr+=pitch-width*4;
     }
}
