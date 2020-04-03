/* channels */
#ifndef	_MBOX_H
#define	_MBOX_H
#define MBOX_CH_POWER   0
#define MBOX_CH_FB      1
#define MBOX_CH_VUART   2
#define MBOX_CH_VCHIQ   3
#define MBOX_CH_LEDS    4
#define MBOX_CH_BTNS    5

#define MBOX_CH_TOUCH   6
#define MBOX_CH_COUNT   7
#define MBOX_CH_PROP    8
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

#define GET_BOARD_REVISION  0x00010002
#define GET_VC_MEMORY       0x00010006
#define REQUEST_CODE        0x00000000
#define REQUEST_SUCCEED     0x80000000
#define REQUEST_FAILED      0x80000001
#define TAG_REQUEST_CODE    0x00000000
#define END_TAG             0x00000000
#define MBOX_TAG_SETCLKRATE 0x00038002

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
#define MBOX_TAG_GETSERIAL      0x10004
#define MBOX_TAG_LAST           0
void get_board_revision();
int mbox_call(unsigned char ch);
void get_vc_base_address();
unsigned long framebuffer_init();
void write_buf(unsigned long location);
volatile unsigned int  __attribute__((aligned(16))) mbox[36];

#endif  /*_MBOX_H */