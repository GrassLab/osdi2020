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
#include "uart.h"


/* mailbox message buffer */
//16 bit,内存对齐，指定内存对齐16字节
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

void get_board_revision(){
    // get the board's unique serial number with a mailbox call
    mbox[0] = 8*4;                  // length of the message
    mbox[1] = MBOX_REQUEST;         // this is a request message
    
    mbox[2] = MBOX_TAG_BOARD_VERSION;   // get serial number command
    mbox[3] = 8;                    // buffer size
    mbox[4] = 0;
    mbox[5] = 0;                    // clear output buffer
    mbox[6] = 0;

    mbox[7] = MBOX_TAG_LAST;

    // send the message to the GPU and receive answer
    if (mbox_call(MBOX_CH_PROP)) {
        uart_puts("My board revision is: ");
        // uart_hex(mbox[6]);
        uart_hex(mbox[5]);
        uart_puts("\n");
    } else {
        uart_puts("Unable to query serial!\n");
    }
}

void set_UART_clock(){
    mbox[0] = 11*4;                  // length of the message
    mbox[1] = MBOX_REQUEST;         // this is a request message
    
    mbox[2] = MBOX_TAG_SET_UART_CLK;   // get serial number command
    mbox[3] = 12;                    // buffer size
    mbox[4] = 0;
    mbox[5] = 0x000000002;                    
    mbox[6] = 0x00001000;
    mbox[7] = 1;
    mbox[8] = 0;
    mbox[9] = 0;

    mbox[10] = MBOX_TAG_LAST;

    if (mbox_call(MBOX_CH_PROP)) {
        uart_puts("set uart clock is: 0x00001000");
        uart_puts("\n");
    } else {
        uart_puts("Unable to query serial!\n");
    }
    get_UART_clock();

    mbox[0] = 11*4;                  // length of the message
    mbox[1] = MBOX_REQUEST;         // this is a request message
    
    mbox[2] = MBOX_TAG_SET_UART_CLK;   // get serial number command
    mbox[3] = 12;                    // buffer size
    mbox[4] = 0;
    mbox[5] = 0x000000002;                    
    mbox[6] = 0x003D0900;
    mbox[7] = 1;
    mbox[8] = 0;
    mbox[9] = 0;

    mbox[10] = MBOX_TAG_LAST;

    if (mbox_call(MBOX_CH_PROP)) {
        uart_puts("set uart clock is: 4M HZ");
        uart_puts("\n");
    } else {
        uart_puts("Unable to query serial!\n");
    }
}

void get_UART_clock(){
    mbox[0] = 9*4;                  // length of the message
    mbox[1] = MBOX_REQUEST;         // this is a request message
    
    mbox[2] = MBOX_TAG_GET_UART_CLK;   // get serial number command
    mbox[3] = 8;                    // buffer size
    mbox[4] = 0;
    mbox[5] = 0x000000002;
    mbox[6] = 0;                    // clear output buffer
    mbox[7] = 0;

    mbox[8] = MBOX_TAG_LAST;

    if (mbox_call(MBOX_CH_PROP)) {
        uart_puts("My uart clock is: 0x");
        uart_hex(mbox[6]);
        uart_puts("\n");
    } else {
        uart_puts("Unable to query serial!\n");
    }
}

void get_vc_memory(){
    mbox[0] = 8*4;                  // length of the message
    mbox[1] = MBOX_REQUEST;         // this is a request message
    
    mbox[2] = MBOX_TAG_VC_MEMORY;   // get serial number command
    mbox[3] = 8;                    // buffer size
    mbox[4] = 0;
    mbox[5] = 0;                    // clear output buffer
    mbox[6] = 0;

    mbox[7] = MBOX_TAG_LAST;

    if (mbox_call(MBOX_CH_PROP)) {
        uart_puts("My VC core base address is: 0x");
        uart_hex(mbox[5]);
        uart_puts(" size is: 0x");
        uart_hex(mbox[6]);
        uart_puts("\n");
    } else {
        uart_puts("Unable to query serial!\n");
    }
}

/**
 * Make a mailbox call. Returns 0 on failure, non-zero on success
 */
int mbox_call(unsigned char ch)
{
    //only check last four digit
    unsigned int r = (((unsigned int)((unsigned long)&mbox)&~0xF) | (ch&0xF));
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
