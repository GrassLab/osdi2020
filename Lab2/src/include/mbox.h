#ifndef _MBOX_H
#define _MBOX_H

#define MAILBOX_EMPTY   0x40000000
#define MAILBOX_FULL    0x80000000
#define MBOX_RESPONSE   0x80000000

#define GET_BOARD_REVISION  0x00010002
#define REQUEST_CODE        0x00000000
#define REQUEST_SUCCEED     0x80000000
#define REQUEST_FAILED      0x80000001
#define TAG_REQUEST_CODE    0x00000000
#define END_TAG             0x00000000

int mbox_call(unsigned char ch);

#endif /*_MBOX_H */
