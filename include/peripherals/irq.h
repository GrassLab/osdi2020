#include "peripherals/mmio.h"

#ifndef IRQ_H
#define IRQ_H

#define IRQ_BASE                (MMIO_BASE + 0xb000)

#define IRQ_BASIC_PENDING       ((unsigned int*)(IRQ_BASE + 0x200))
#define IRQ_ENABLE_2            ((unsigned int*)(IRQ_BASE + 0x214))

#endif