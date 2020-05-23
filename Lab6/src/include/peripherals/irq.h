#ifndef _P_IRQ_H
#define _P_IRQ_H

#include "base.h"
#include "../mm.h"

#define IRQ_PENDING_1 (PBASE + 0x0000B204)
#define IRQ_PENDING_2 (PBASE + 0x0000B208)
#define CORE_SOURCE_0 (PBASE + 0x01000060)

#endif
