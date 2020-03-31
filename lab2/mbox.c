/*
 * Copyright (C) 2018 bzt (bztsrc@github)
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */

#include "gpio.h"
#include "mbox.h"

/* mailbox message buffer */
volatile unsigned int  __attribute__((aligned(16))) mbox[36];

#define VIDEOCORE_MBOX  (MMIO_BASE+0x0000B880)
#define MBOX_READ       ((volatile unsigned int*)(VIDEOCORE_MBOX+0x0))
#define MBOX_POLL       ((volatile unsigned int*)(VIDEOCORE_MBOX+0x10))
#define MBOX_SENDER     ((volatile unsigned int*)(VIDEOCORE_MBOX+0x14))
#define MBOX_STATUS     ((volatile unsigned int*)(VIDEOCORE_MBOX+0x18))
#define MBOX_CONFIG     ((volatile unsigned int*)(VIDEOCORE_MBOX+0x1C))
#define MBOX_WRITE      ((volatile unsigned int*)(VIDEOCORE_MBOX+0x20))
#define MBOX_RESPONSE   0x80000000
#define MBOX_FULL       0x80000000
#define MBOX_EMPTY      0x40000000

/**
 * Make a mailbox call. Returns 0 on failure, non-zero on success
 */
int mbox_call(unsigned char ch)
{
    // write data: 32 bits = | 28 bits for data | 4 bits for channel |
    unsigned int r = (((unsigned int)((unsigned long)&mbox)&~0xF) | (ch&0xF));
    // read status register until the full flag is not set
    do{asm volatile("nop");}while(*MBOX_STATUS & MBOX_FULL);
    /* write the address of our message to the mailbox with channel identifier */
    *MBOX_WRITE = r;

    /* now wait for the response */
    while(1) {
        // read the status register until the empty flag is not set
        do{asm volatile("nop");}while(*MBOX_STATUS & MBOX_EMPTY);
        
        // read data from read register
        if(r == *MBOX_READ)
            // if the lower four bits do not match the channel number desired then repeat from 1 
            return mbox[1]==MBOX_RESPONSE;
    }
    return 0;
}

void set_board_ver_mbox(){
    mbox[0] = 8*4;                  
    mbox[1] = MBOX_REQUEST;         
    
    mbox[2] = MBOX_BOARD_REVISION;
    mbox[3] = 8;                    
    mbox[4] = TAG_REQUEST_CODE;
    mbox[5] = 0;                    
    mbox[6] = 0;

    mbox[7] = MBOX_TAG_LAST;
}

void set_VC_Mem_base_size_mbox(){
    mbox[0] = 8*4;                  
    mbox[1] = MBOX_REQUEST;         

    mbox[2] = MBOX_VC_MEM_BASE;
    mbox[3] = 8;                    
    mbox[4] = TAG_REQUEST_CODE;
    mbox[5] = 0;                    
    mbox[6] = 0;

    mbox[7] = MBOX_TAG_LAST;
}

void set_queryUART0_mbox(){
    mbox[0] = 8*4;                  
    mbox[1] = MBOX_REQUEST;         

    mbox[2] = MBOX_POWER_STATE;
    mbox[3] = 8;                    
    mbox[4] = TAG_REQUEST_CODE;
    mbox[5] = PWID_UART0;
    mbox[6] = 0;

    mbox[7] = MBOX_TAG_LAST;
}

void set_queryUART1_mbox(){
    mbox[0] = 8*4;                  
    mbox[1] = MBOX_REQUEST;         

    mbox[2] = MBOX_POWER_STATE;
    mbox[3] = 8;                    
    mbox[4] = TAG_REQUEST_CODE;
    mbox[5] = PWID_UART1;
    mbox[6] = 0;

    mbox[7] = MBOX_TAG_LAST;
}

void set_uart0_clock_rate_mbox(){
    mbox[0] = 9*4;
    mbox[1] = MBOX_REQUEST;
    mbox[2] = MBOX_TAG_SETCLKRATE; 
    mbox[3] = 12;
    mbox[4] = TAG_REQUEST_CODE;
    mbox[5] = 2;                    // clock id
    mbox[6] = 4000000;              // rate (in Hz)
    mbox[7] = 0;                    // skip setting tutbo
    mbox[8] = MBOX_TAG_LAST;
}

//linear framebuffer
void set_lfb_init_mbox(){
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
}