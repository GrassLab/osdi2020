#ifndef MAILBOX_H_
#define MAILBOX_H_

#include "base.h"
#include "types.h"

#define MAILBOX_BASE (GPU_PERIPHERAL_BASE + 0xb880)
#define MAILBOX_READ   ((volatile uint32_t *)(MAILBOX_BASE))
#define MAILBOX_STATUS ((volatile uint32_t *)(MAILBOX_BASE + 0x18))
#define MAILBOX_WRITE  ((volatile uint32_t *)(MAILBOX_BASE + 0x20))
#define MAILBOX_EMPTY 0x40000000
#define MAILBOX_FULL 0x80000000

uint32_t get_board_revision(void);
uint64_t get_vc_memory(void);
void mailbox_call(uint32_t *mailbox);

#endif // MAILBOX_H_
