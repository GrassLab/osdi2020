#ifndef _P_MBOX_H
#define _P_MBOX_H

#include "base.h"

#define MAILBOX_BASE    PBASE + 0xb880

#define MAILBOX_READ    MAILBOX_BASE
#define MAILBOX_STATUS  MAILBOX_BASE + 0x18
#define MAILBOX_WRITE   MAILBOX_BASE + 0x20

#endif /*_P_MBOX_H */
