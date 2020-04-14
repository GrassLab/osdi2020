#ifndef __TIMER_H__
#define __TIMER_H__

void arm_local_timer_init( void );
void arm_local_timer_handler( void );
void core_timer_enable( void );
void core_timer_handler( void );

#endif