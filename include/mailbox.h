#ifndef _MAILBOX_H_
#define _MAILBOX_H_
#include "mm.h"

/* channels */
#define MBOX_CH_FB      1
#define MBOX_CH_PROP    8

#define GET_BOARD_MODEL     0x00010001
#define GET_BOARD_REVISION  0x00010002
#define GET_BOARD_SERIAL    0x00010004
#define GET_VC_MEMORY       0x00010006
#define SET_CLK_RATE        0x00038002

#define REQUEST_CODE        0x00000000
#define REQUEST_SUCCEED     0x80000000
#define REQUEST_FAILED      0x80000001
#define TAG_REQUEST_CODE    0x00000000
#define END_TAG             0x00000000

int mbox_call(volatile uint32_t *message, uint8_t channel);

uint32_t get_board_revision();

uint32_t get_vc_memory();

#endif
