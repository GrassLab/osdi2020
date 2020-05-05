#include "stdint.h"

#ifndef __EXC_H__
#define __EXC_H__

struct trapframe {
    uint64_t x[31]; // general register from x0 ~ x30
    uint64_t sp_el0;
    uint64_t elr_el1;
    uint64_t spsr_el1;
};

#endif

extern uint64_t arm_core_timer_jiffies, arm_local_timer_jiffies;

void irq_enable();
void arm_core_timer_enable();
void arm_core_timer_disable();
void arm_local_timer_enable();
void arm_local_timer_disable();