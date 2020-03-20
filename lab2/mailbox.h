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

/* calculate mailbox buffer size with single tag, tag buffer in bytes */
#define MAILBOX_SINGLE_BUFFER_SIZE(req,resp) (24 + req + resp)

extern volatile uint32_t __attribute__((aligned(16))) __mailbox_buffer[0x40];

int send_buffer_to_mailbox(void);
uint32_t mailbox_get_board_revision(void);
int mailbox_get_vc_memory(void);

#endif

