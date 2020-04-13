#include "peripherals/mmio.h"

#ifndef TIMER_H
#define TIMER_H

#define CORE0_TIMER_IRQ_CTRL    0x40000040
#define CORE1_TIMER_IRQ_CTRL    0x40000044
#define CORE2_TIMER_IRQ_CTRL    0x40000048
#define CORE3_TIMER_IRQ_CTRL    0x4000004C
#define EXPIRE_PERIOD           0xfff

#endif