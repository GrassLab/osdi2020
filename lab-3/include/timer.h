#ifndef TIMER_H
#define TIMER_H

#define SYSTEM_TIMER_COMPARE1 0x3f003010
#define SYSTEM_TIMER_CLO 0x3f003004
#define SYSTEM_TIMER_CS 0x3f003000
#define IRQ_ENABLE0 0x3f00b210

#define LOCAL_TIMER_CONTROL_REG 0x40000034
#define LOCAL_TIMER_IRQ_CLR     0x40000038

#define CORE0_TIMER_IRQ_CTRL 0x40000040
#define CORE0_EXPIRE_PERIOD 0xfffffff

void local_timer_init();
void local_timer_handler();

#endif