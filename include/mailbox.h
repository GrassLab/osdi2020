#ifndef _MAILBOX_H_
#define _MAILBOX_H_
#include "mm.h"

enum {
    VIDEOCORE_MBOX      = (MMIO_BASE+0x0000B880),
    MBOX_READ           = (VIDEOCORE_MBOX+0x0),
    MBOX_POLL           = (VIDEOCORE_MBOX+0x10),
    MBOX_SENDER         = (VIDEOCORE_MBOX+0x14),
    MBOX_STATUS         = (VIDEOCORE_MBOX+0x18),
    MBOX_CONFIG         = (VIDEOCORE_MBOX+0x1C),
    MBOX_WRITE          = (VIDEOCORE_MBOX+0x20),
    MBOX_RESPONSE       = 0x80000000,
    MBOX_FULL           = 0x80000000,
    MBOX_EMPTY          = 0x40000000,

    MBOX_CH_FB          = 1,
    MBOX_CH_PROP        = 8,

    GET_BOARD_MODEL     = 0x00010001,
    GET_BOARD_REVISION  = 0x00010002,
    GET_BOARD_SERIAL    = 0x00010004,
    GET_VC_MEMORY       = 0x00010006,
    SET_CLK_RATE        = 0x00038002,

    REQUEST_CODE        = 0x00000000,
    REQUEST_SUCCEED     = 0x80000000,
    REQUEST_FAILED      = 0x80000001,
    TAG_REQUEST_CODE    = 0x00000000,
    END_TAG             = 0x00000000,
};

int mbox_call(volatile unsigned int *message, uint8_t channel);

uint32_t get_board_revision();

uint32_t get_vc_memory();

#endif
