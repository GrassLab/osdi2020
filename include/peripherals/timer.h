#include "peripherals/mmio.h"

#ifndef ARM_CORE_TIMER_H
#define ARM_CORE_TIMER_H

#define CORE0_TIMER_IRQ_CTRL    ((unsigned int*)0x40000040)
#define CORE1_TIMER_IRQ_CTRL    ((unsigned int*)0x40000044)
#define CORE2_TIMER_IRQ_CTRL    ((unsigned int*)0x40000048)
#define CORE3_TIMER_IRQ_CTRL    ((unsigned int*)0x4000004C)

#define CORE0_INTR_SRC          ((unsigned int*)0x40000060)
#define CORE1_INTR_SRC          ((unsigned int*)0x40000064)
#define CORE2_INTR_SRC          ((unsigned int*)0x40000068)
#define CORE3_INTR_SRC          ((unsigned int*)0x4000006C)

#define EXPRIED_PERIOD          0xffffff

#endif