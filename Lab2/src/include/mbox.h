#ifndef _MBOX_H
#define _MBOX_H

#define MAILBOX_EMPTY   0x40000000
#define MAILBOX_FULL    0x80000000

#define GET_BOARD_REVISION  0x00010002
#define GET_VC_MEMORY       0x00010006
#define MBOX_TAG_SETCLKRATE 0x00038002

#define REQUEST_CODE        0x00000000
#define REQUEST_SUCCEED     0x80000000
#define REQUEST_FAILED      0x80000001
#define TAG_REQUEST_CODE    0x00000000
#define END_TAG             0x00000000

extern unsigned int mbox[36];
int mbox_call(unsigned char ch);

#endif /*_MBOX_H */
