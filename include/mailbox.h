#ifndef MAILBOX_H
#define MAILBOX_H

#include "gpio.h"
#include "type.h"

#define MAILBOX_BASE        (MMIO_BASE + 0x0000B880)

/** mailbox register  */
/** https://github.com/raspberrypi/firmware/wiki/Mailboxes#mailbox-registers */
#define MAILBOX_REG_READ    ((volatile unsigned int*)(MAILBOX_BASE + 0x00000000))
#define MAILBOX_REG_STATUS  ((volatile unsigned int*)(MAILBOX_BASE + 0x00000018))
#define MAILBOX_REG_WRITE   ((volatile unsigned int*)(MAILBOX_BASE + 0x00000020))

/** mailbox channel */
/* https://github.com/raspberrypi/firmware/wiki/Mailboxes#channels */
#define MAILBOX_CH_PPOWER   0
#define MaILBOX_CH_FB       1
#define MAILBOX_CH_V_UART   2
#define MAILBOX_CH_VcHIQ    3
#define MAILBOX_CH_LED      4
#define MAILBOX_CH_BTN      5
#define MAILBOX_CH_TOUCH    6
#define MAILBOX_CH_RESEVRED 7
#define MAILBOX_CH_PROP     8

/** mailbox status  */
#define MAILBOX_EMPTY       0x40000000
#define MAILBOX_FULL        0x80000000

/** tags */
#define TAGS_REQ_CODE       0x00000000
#define TAGS_REQ_SUCCEED    0x80000000
#define TAGS_REQ_FAILED     0x80000001
#define TAGS_END            0x00000000

/** tags operator */
#define TAGS_BOARD_MODEL    0x00010001
#define TAGS_BOARD_REVISION 0x00010002
#define TAGS_MAC_ADDR       0x00010003
#define TAGS_BOARD_SERIAL   0x00010004
#define TAGS_ARM_MEM        0x00010005
#define TAGS_VC_MEM         0x00010006
#define TAGS_CLOCKS         0x00010007

uint32_t mbox_get_board_revision ();
uint64_t mbox_get_VC_base_addr ();

#endif