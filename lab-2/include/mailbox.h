#ifndef MAILBOX_H
#define MAILBOX_H

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

/* screen related */
#define SET_PHYSICAL_DISPLAY 0x00048003
#define SET_VIRTUAL_DISPLAY  0x00048004
#define SET_DEPTH            0x00048005
#define ALLOCATE_BUFFER      0x00040001
#define VIRTUAL_OFFSET       0x00048009
#define SET_PIXEL_ORDER      0x00048009
#define FB_WIDTH             640
#define FB_HEIGHT            480
#define BITS_PER_PIXEL       32

/* tags */
#define GET_BOARD_REVISION      0x00010002
#define GET_SERIAL_NUMBER       0x00010004
#define GET_VC_MEMORY           0x00010006
#define MBOX_TAG_SETCLKRATE     0x00038002
#define MBOX_TAG_GETCLKRATE     0x00030002
#define TAG_REQUEST_CODE        0x00000000
#define TAG_END                 0x00000000

int mbox_call(unsigned char ch);
void print_serial_number();
void print_board_revision();
void print_vc_base_address();
void print_vc_size();

#endif