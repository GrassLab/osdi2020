#ifndef __TIMER_H__
#define __TIMER_H__

void local_timer_init();
void local_timer_handler();

void sys_timer_init();
void sys_timer_handler();

void core_timer_init();
void core_timer_handler();

#endif
