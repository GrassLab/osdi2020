#ifndef	_TIMER_H
#define	_TIMER_H

void timer_init ( void );
void handle_timer_irq ( void );

void sched_core_timer_handler();
void sys_core_timer_enable();

#endif  /*_TIMER_H */
