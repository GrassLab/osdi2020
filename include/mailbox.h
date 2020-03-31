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

/** hardware tags operator */
#define TAGS_HARDWARE_BOARD_MODEL    0x00010001
#define TAGS_HARDWARE_BOARD_REVISION 0x00010002
#define TAGS_HARDWARE_MAC_ADDR       0x00010003
#define TAGS_HARDWARE_BOARD_SERIAL   0x00010004
#define TAGS_HARDWARE_ARM_MEM        0x00010005
#define TAGS_HARDWARE_VC_MEM         0x00010006
#define TAGS_HARDWARE_CLOCKS         0x00010007

/** clock id */
#define CLOCK_ID_RESERVED   0x000000000       
#define CLOCK_ID_EMMC       0x000000001   
#define CLOCK_ID_UART       0x000000002   
#define CLOCK_ID_ARM        0x000000003          
#define CLOCK_ID_CORE       0x000000004   
#define CLOCK_ID_V3D        0x000000005   
#define CLOCK_ID_H264       0x000000006   
#define CLOCK_ID_ISP        0x000000007   
#define CLOCK_ID_SDRAM      0x000000008   
#define CLOCK_ID_PIXEL      0x000000009   
#define CLOCK_ID_PWM        0x00000000a   
#define CLOCK_ID_HEVC       0x00000000b   
#define CLOCK_ID_EMMC2      0x00000000c   
#define CLOCK_ID_M2MC       0x00000000d   
#define CLOCK_ID_PIXEL_BVB  0x00000000e   

/** clock tags operator */
#define TAGS_GET_CLOCK      0x00030002
#define TAGS_SET_CLOCK      0x00038002

uint32_t mbox_get_board_revision ();
uint64_t mbox_get_VC_base_addr ();
void mbox_set_clock_to_PL011 ();

#endif