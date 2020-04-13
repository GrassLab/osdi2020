#ifndef __TIME_H_
#define __TIME_H_

#include "types.h"
#include "mm.h"

enum {
    LOCAL_TIMER_CONTROL_REG = 0x40000034,
    LOCAL_TIMER_IRQ_CLR     = 0x4000003
};

#define EXPIRE_PERIOD           0xFFFFFFF
#define CORE0_TIMER_IRQ_CTRL    0x40000040

void core_timer_enable();

void core_timer_handler();

void local_timer_init();

void local_timer_handler();

void delay(int32_t count);

float get_timestamp();

#endif
