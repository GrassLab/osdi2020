#ifndef _TIMER_H_
#define _TIMER_H_
#define CORE0_TIMER_IRQ_CTRL (volatile unsigned int *)0x40000040
#define LOCAL_TIMER_CONTROL_REG (volatile unsigned int *)0x40000034
#define LOCAL_TIMER_IRQ_CLR (volatile unsigned int *)0x40000038

extern void _core_timer_enable();
extern void _core_timer_disable();
extern void _core_timer_handler(int);

extern void local_timer_enable();
extern void local_timer_disable();
extern void local_timer_handler();
#endif