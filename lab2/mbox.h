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

/* a properly aligned buffer */
extern volatile unsigned int mbox[36];

#define MBOX_REQUEST    0

/* channels */
#define MBOX_CH_POWER   0
#define MBOX_CH_FB      1
#define MBOX_CH_VUART   2
#define MBOX_CH_VCHIQ   3
#define MBOX_CH_LEDS    4
#define MBOX_CH_BTNS    5
#define MBOX_CH_TOUCH   6
#define MBOX_CH_COUNT   7
#define MBOX_CH_PROP    8

/* tags */
#define MBOX_TAG_GETSERIAL      0x10004
#define MBOX_TAG_SETCLKRATE     0x38002
#define MBOX_TAG_LAST           0
#define GET_BOARD_REVISION      0x00010002
#define GET_VC_MEMORY           0x00010006

int mbox_call(unsigned char ch);

void get_board_revision() {
    // unsigned int mailbox[7];
    mbox[0] = 7 * 4; // buffer size in bytes
    mbox[1] = MBOX_REQUEST;
    // tags begin
    mbox[2] = GET_BOARD_REVISION; // tag identifier
    mbox[3] = 4; // maximum of request and response value buffer's length.
    mbox[4] = 0;
    mbox[5] = 0; // value buffer
    // tags end
    mbox[6] = MBOX_TAG_LAST;

    mbox_call(MBOX_CH_PROP); // message passing procedure call, you should implement it following the 6 steps provided above.

	uart_puts("Board Revision: ");
    uart_hex(mbox[5]);
	uart_puts("\r\n");
    // printf("0x%x\n", mailbox[5]); // it should be 0xa020d3 for rpi3 b+
}

void get_vc_core_base_addr() {
    // unsigned int mailbox[7];
    mbox[0] = 8 * 4; // buffer size in bytes
    mbox[1] = MBOX_REQUEST;
    // tags begin
    mbox[2] = GET_VC_MEMORY; // tag identifier
    mbox[3] = 8; // maximum of request and response value buffer's length.
    mbox[4] = 0;
    mbox[5] = 0; // value buffer
    // tags end
    mbox[6] = MBOX_TAG_LAST;

    mbox_call(MBOX_CH_PROP); // message passing procedure call, you should implement it following the 6 steps provided above.

	uart_puts("VC Core Base Address: ");
    uart_hex(mbox[5]);
    uart_puts(" Size: ");
    uart_hex(mbox[6]);
	uart_puts("\r\n");
}