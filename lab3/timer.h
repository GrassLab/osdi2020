#ifndef __TIMER_H__
#define __TIMER_H__

extern void timer_enable_core_timer(void);
extern void timer_disable_core_timer(void);
extern void timer_set_core_timer(int sec);
extern void timer_expire_core_timer(void);

#endif

