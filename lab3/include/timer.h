#ifndef	_TIMER_H
#define	_TIMER_H

#include "base.h"

#define TIMER_CS        (PBASE+0x00003000)
#define TIMER_CLO       (PBASE+0x00003004)
#define TIMER_CHI       (PBASE+0x00003008)
#define TIMER_C0        (PBASE+0x0000300C)
#define TIMER_C1        (PBASE+0x00003010)
#define TIMER_C2        (PBASE+0x00003014)
#define TIMER_C3        (PBASE+0x00003018)

#define TIMER_CS_M0	(1 << 0)
#define TIMER_CS_M1	(1 << 1)
#define TIMER_CS_M2	(1 << 2)
#define TIMER_CS_M3	(1 << 3)

#define LOCAL_TIMER_CONTROL_REG ((volatile unsigned int*)0x40000034)
#define LOCAL_TIMER_IRQ_CLR ((volatile unsigned int*)0x40000038)

void timer_init ();
void handle_sys_timer_irq();
void local_timer_init();

void core_timer_enable();
void clean_core_timer();
void core_timer_disable();
void disable_timer_controller();
void disable_all_timer();
void write_cntp_tval(unsigned int val);
unsigned int read_cntfrq(void);
unsigned int read_cntp_tval(void);

#endif  /*_TIMER_H */