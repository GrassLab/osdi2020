#ifndef TIMER_H
#define TIMER_H

#define CORE0_TIMER_IRQ_CTRL    ((volatile unsigned int*)0x40000040)
#define CORE1_TIMER_IRQ_CTRL    ((volatile unsigned int*)0x40000044)
#define CORE2_TIMER_IRQ_CTRL    ((volatile unsigned int*)0x40000048)
#define CORE3_TIMER_IRQ_CTRL    ((volatile unsigned int*)0x4000004C)

#define LOCAL_TIMER_CTRL ((volatile unsigned int*)0x40000034)
#define LOCAL_TIMER_CLR  ((volatile unsigned int*)0x40000038)

void core_timer_enable ();
void core_timer_reload ();

void local_timer_enable ();
void local_timer_reload ();

#endif