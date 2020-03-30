#include "meta_macro.h"
#include <stdint.h>

#ifndef __MAILBOX_H__
#define __MAILBOX_H__

#define MAILBOX_PROPERTY_ARM_TO_VC 8

#define MAILBOX_BASE                    (PERIPHERAL_TO_PHYSICAL(0x7E00B880))
#define MAILBOX_READ                    ((uint32_t *)(MAILBOX_BASE))
#define MAILBOX_STATUS                  ((uint32_t *)(MAILBOX_BASE + 0x18))
#define MAILBOX_WRITE                   ((uint32_t *)(MAILBOX_BASE + 0x20))

#define MAILBOX_EMPTY                   0x40000000
#define MAILBOX_FULL                    0x80000000

#define MAILBOX_BUFFER_REQUEST_CODE     0x00000000
#define MAILBOX_BUFFER_REQUEST_SUCCEED  0x80000000
#define MAILBOX_BUFFER_REQUEST_FAILED   0x80000001

#define MAILBOX_TAG_REQUEST_CODE        0x00000000
#define MAILBOX_TAG_END                 0x00000000

#define MAILBOX_GET_BOARD_REVISION      0x00010002
#define MAILBOX_GET_BOARD_REVISION_REQ  0x0
#define MAILBOX_GET_BOARD_REVISION_RESP 0x4
#define MAILBOX_GET_VC_MEMORY           0x00010006
#define MAILBOX_GET_VC_MEMORY_REQ       0x0
#define MAILBOX_GET_VC_MEMORY_RESP      0x8

#define MAILBOX_SET_CLOCK_RATE          0x00038002
#define MAILBOX_SET_CLOCK_RATE_REQ      12
#define MAILBOX_SET_CLOCK_RATE_RESP     8

#define MAILBOX_SET_PHYSICAL_WH         0x00048003
#define MAILBOX_SET_PHYSICAL_WH_REQ     0x8
#define MAILBOX_SET_PHYSICAL_WH_RESP    0x8
#define MAILBOX_SET_VIRTUAL_WH          0x00048004
#define MAILBOX_SET_VIRTUAL_WH_REQ      0x8
#define MAILBOX_SET_VIRTUAL_WH_RESP     0x8
#define MAILBOX_SET_VIRTUAL_OFFSET      0x00048009
#define MAILBOX_SET_VIRTUAL_OFFSET_REQ  0x8
#define MAILBOX_SET_VIRTUAL_OFFSET_RESP 0x8
#define MAILBOX_SET_DEPTH               0x00048005
#define MAILBOX_SET_DEPTH_REQ           0x4
#define MAILBOX_SET_DEPTH_RESP          0x4
#define MAILBOX_SET_PIXEL_ORDER         0x00048006
#define MAILBOX_SET_PIXEL_ORDER_REQ     0x4
#define MAILBOX_SET_PIXEL_ORDER_RESP    0x4
#define MAILBOX_GET_FRAMEBUFFER         0x00040001
#define MAILBOX_GET_FRAMEBUFFER_REQ     0x4
#define MAILBOX_GET_FRAMEBUFFER_RESP    0x8
#define MAILBOX_GET_PITCH               0x00040008
#define MAILBOX_GET_PITCH_REQ           0x0
#define MAILBOX_GET_PITCH_RESP          0x4


#define MAILBOX_UART_ID                 0x2

/* calculate mailbox buffer size with single tag, tag buffer in bytes */
#define MAX(a, b) (((a)>(b))?(a):(b))
#define MAILBOX_SINGLE_BUFFER_SIZE(req,resp) (24 + MAX(req, resp))

extern volatile uint32_t __attribute__((aligned(16))) __mailbox_buffer[0x40];

int send_buffer_to_mailbox(void);
uint32_t mailbox_get_board_revision(void);
int mailbox_get_vc_memory(void);
int mailbox_set_clock_rate(uint32_t device_id, uint32_t clock_rate);
int mailbox_framebuffer_init(uint32_t width, uint32_t height, uint32_t depth);

#endif

