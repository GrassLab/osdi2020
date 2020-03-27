#ifndef _MBOX_H
#define _MBOX_H

#define MAILBOX_EMPTY   0x40000000
#define MAILBOX_FULL    0x80000000

#define GET_BOARD_REVISION  0x00010002
#define GET_VC_MEMORY       0x00010006
#define MBOX_TAG_SETCLKRATE 0x00038002

#define SET_PHY_DISPLAY         0x00048003
#define SET_VIR_DISPLAY         0x00048004
#define SET_VIR_OFFSET          0x00048009
#define SET_DEPTH               0x00048005
#define SET_PIXEL_ORDER         0x00048006
#define ALLOCATE_BUFFER         0x00040001
#define GET_PITCH               0x00040008


#define REQUEST_CODE        0x00000000
#define REQUEST_SUCCEED     0x80000000
#define REQUEST_FAILED      0x80000001
#define END_TAG             0x00000000

extern unsigned int mbox[36];
int mbox_call(unsigned char ch);

#endif /*_MBOX_H */
