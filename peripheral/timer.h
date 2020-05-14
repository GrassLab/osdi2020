#ifndef TIMER
#define TIMER

#include <stddef.h>
#include <mm.h>
#define CORE0_TIMER_IRQ_CTRL (unsigned int *)(MMIO_BASE+0x1000040)
#define LOCAL_TIMER_CONTROL_REG (unsigned int *)(MMIO_BASE+0x1000034)
#define LOCAL_TIMER_IRQ_CLR (unsigned int *)(MMIO_BASE+0x1000038)

extern void sys_core_timer_enable ();
extern void core_timer_handler ();
void core_timer_enable ();
void local_timer_handler ();
void local_timer_init ();
double get_time ();
void sys_get_time (size_t *cnt, size_t *freq);
#endif /* ifndef TIMER */
