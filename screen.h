/*****

Document:
https://github.com/raspberrypi/firmware/wiki/Mailbox-property-interface

*****/
#ifndef _SCREEN_H_ 
#define _SCREEN_H_
#include "uart.h"
#include "mailbox.h"

#define GET_BOARD_REVISION  0x00010002
#define GET_VC_MEMORY       0x00010006
#define REQUEST_CODE        0x00000000
#define REQUEST_SUCCEED     0x80000000
#define REQUEST_FAILED      0x80000001
#define TAG_REQUEST_CODE    0x00000000
#define END_TAG             0x00000000


unsigned int width, height, pitch, isrgb;   /* dimensions and channel order */
unsigned char *lfb;                         /* raw frame buffer address */


void get_board_revision(){
    mailbox[0] = 7*4;                  // length of the message
    mailbox[1] = REQUEST_CODE;         // this is a request message
    
    mailbox[2] = GET_BOARD_REVISION;   // get serial number command
    mailbox[3] = 4;                    // buffer size
    mailbox[4] = TAG_REQUEST_CODE;
    mailbox[5] = 0;                    // clear output buffer

    mailbox[6] = END_TAG;

    // send the message to the GPU and receive answer
    if (mailbox_call(MBOX_CH_PROP)) {
        uart_puts("Board Revision:\t\t");

        // it should be 0xa020d3 for rpi3 B+
        // in qemu, show 0xa02082 for rpi3 B Revision:1.2 RAM:1GB Sony UK
        // https://www.raspberrypi.org/documentation/hardware/raspberrypi/revision-codes/README.md
        uart_send_hex(mailbox[5]);
        uart_puts("\n");
    } else {
        uart_puts("MailBox fail\n");
    }
}

void get_vc_memory(){
    mailbox[0] = 8 * 4;
    mailbox[1] = REQUEST_CODE;

    mailbox[2] = GET_VC_MEMORY;
    mailbox[3] = 8;
    mailbox[4] = TAG_REQUEST_CODE;
    mailbox[5] = 0;
    mailbox[6] = 0;

    mailbox[7] = END_TAG;

    if(mailbox_call(MBOX_CH_PROP)){
        uart_puts("VC Core base address:\t");
        uart_send_hex(mailbox[5]);
        uart_puts("\n");
        uart_puts("VC memory size:\t\t");
        uart_send_hex(mailbox[6]);
        uart_puts("\n");
    }

}

#define HEADER_PIXEL(data,pixel) {\
pixel[0] = (((data[0] - 33) << 2) | ((data[1] - 33) >> 4)); \
pixel[1] = ((((data[1] - 33) & 0xF) << 4) | ((data[2] - 33) >> 2)); \
pixel[2] = ((((data[2] - 33) & 0x3) << 6) | ((data[3] - 33))); \
data += 4; \
}

void showpicture();

void get_frame_buffer(){

    mailbox[0] = 35 * 4;
    mailbox[1] = REQUEST_CODE;

    mailbox[2] = 0x48003;  //set phy wh
    mailbox[3] = 8;
    mailbox[4] = 8;
    mailbox[5] = 1024;         //FrameBufferInfo.width
    mailbox[6] = 768;          //FrameBufferInfo.height

    mailbox[7] = 0x48004;  //set virt wh
    mailbox[8] = 8;
    mailbox[9] = 8;
    mailbox[10] = 1024;        //FrameBufferInfo.virtual_width
    mailbox[11] = 768;         //FrameBufferInfo.virtual_height

    mailbox[12] = 0x48009; //set virt offset
    mailbox[13] = 8;
    mailbox[14] = 8;
    mailbox[15] = 0;           //FrameBufferInfo.x_offset
    mailbox[16] = 0;           //FrameBufferInfo.y.offset

    mailbox[17] = 0x48005; //set depth
    mailbox[18] = 4;
    mailbox[19] = 4;
    mailbox[20] = 32;          //FrameBufferInfo.depth

    mailbox[21] = 0x48006; //set pixel order
    mailbox[22] = 4;
    mailbox[23] = 4;
    mailbox[24] = 1;           //RGB, not BGR preferably

    mailbox[25] = 0x40001;     //get framebuffer, gets alignment on request
    mailbox[26] = 8;
    mailbox[27] = 8;
    mailbox[28] = 4096;        //FrameBufferInfo.pointer
    mailbox[29] = 0;           //FrameBufferInfo.size
    
    
    mailbox[30] = 0x40008; //get pitch
    mailbox[31] = 4;
    mailbox[32] = 4;
    mailbox[33] = 0;           //FrameBufferInfo.pitch

    mailbox[34] = END_TAG;


    if(mailbox_call(MBOX_CH_PROP) && mailbox[20]==32 && mailbox[28]!= 0){
        //convert GPU address to ARM address
        mailbox[28] &= 0x3FFFFFFF;
        width=mailbox[5];          //get actual physical width
        height=mailbox[6];         //get actual physical height
        pitch=mailbox[33];         //get number of bytes per line
        isrgb = mailbox[24];
        lfb = (void*)((unsigned long)(mailbox[28]));;
    }
    else{
        uart_puts("get frame buffer fail\n");
    }

}

void showpicture()
{
    int x,y;
    char pic[128 * 128 * 4];
    char pixel[4];

    int pic_height = 128;
    int pic_width = 128;

    for(int i=0; i< pic_height * pic_width * 4; i++){
        pic[i] = 0xfc;
    }


    for(int zz=0; zz<9; zz++){
        for(int z=0; z<5; z++){
            char *data = pic;
            unsigned char *ptr=lfb;
            ptr += (zz % 2) * pic_height * pitch;
            ptr += (2 * pic_height * z * pitch) + (zz * pic_width * 4);
            // ptr += (height-pic_height)/2*pitch + (width-pic_width)*2;
            // ptr += (height - pic_height/2) * pitch;
            for(y=0; y<pic_height; y++) {
                for(x=0; x<pic_width; x++) {
                    HEADER_PIXEL(data, pixel);
                    *((unsigned int*)ptr)=isrgb ? *((unsigned int *)&pixel) : (unsigned int)(pixel[0]<<16 | pixel[1]<<8 | pixel[2]);
                    ptr+=4;
                }
                ptr+=pitch-pic_width*4;
            }

        }
    
    }


}



#endif
