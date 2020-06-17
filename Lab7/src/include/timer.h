#ifndef _TIMER_H
#define _TIMER_H

void core_timer_enable();
void core_timer_handler();

#ifndef __ASSEMBLER__
void sys_timer_init();
void sys_timer_handler();
#endif

#endif /*_TIMER_H */
