#ifndef __TIMER_H__
#define __TIMER_H__

#define LOCAL_TIMER_CONTROL_REG ((uint32_t *)0x40000034)
#define LOCAL_TIMER_IRQ_CLR ((uint32_t *)0x40000038)

#define LOCAL_TIMER_SECS 1
#define CORE_TIMER_SECS 1

void timer_enable_and_set_local_timer(void);
void timer_disable_local_timer(void);
void timer_clear_local_timer_int_and_reload(void);

extern void timer_enable_core_timer(void);
extern void timer_disable_core_timer(void);
extern void timer_set_core_timer_sec(int sec);
extern void timer_set_core_timer_approx_ms(int microsec);
extern void timer_expire_core_timer(void);

#endif

