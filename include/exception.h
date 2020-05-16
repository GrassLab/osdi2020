#include "stdint.h"

extern uint64_t arm_core_timer_jiffies, arm_local_timer_jiffies;
extern uint64_t cntfrq_el0, cntpct_el0;

void irq_enable();
void arm_core_timer_enable();
void arm_core_timer_disable();
void arm_local_timer_enable();
void arm_local_timer_disable();