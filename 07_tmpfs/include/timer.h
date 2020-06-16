#ifndef _TIMER_H
#define _TIEMR_H

#define CORE0_TIMER_IRQ_CTRL 0x40000040
#define EXPIRE_PERIOD 0xfffff8

#ifndef __ASSEMBLER__
void enable_core_timer();
#endif

#endif
