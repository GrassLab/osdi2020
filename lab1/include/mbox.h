#ifndef __MBOX_H__
#define __MBOX_H__

#include "gpio.h"

#define MBOX_BASE    (MMIO_BASE + 0xb880)

#define MBOX_READ    (MBOX_BASE)
#define MBOX_STATUS  (MBOX_BASE + 0x18)
#define MBOX_WRITE   (MBOX_BASE + 0x20)

#define MBOX_EMPTY   (0x40000000)
#define MBOX_FULL    (0x80000000)

#define MBOX_CH_POWER   0
#define MBOX_CH_FB      1
#define MBOX_CH_VUART   2
#define MBOX_CH_VCHIQ   3
#define MBOX_CH_LEDS    4
#define MBOX_CH_BTNS    5
#define MBOX_CH_TOUCH   6
#define MBOX_CH_COUNT   7
#define MBOX_CH_PROP    8

void get_board_revision();
void get_vc_memaddr();
int mbox_call(unsigned int*, unsigned char);

#define GET_BOARD_REVISION  0x00010002
#define REQUEST_CODE        0x00000000
#define REQUEST_SUCCEED     0x80000000
#define REQUEST_FAILED      0x80000001
#define TAG_REQUEST_CODE    0x00000000
#define END_TAG             0x00000000
#define GET_VC_MEMORY       0x00010006
#define TAG_SETCLKRATE     0x38002
#endif
