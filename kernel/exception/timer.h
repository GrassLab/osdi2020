#ifndef __SYS_TIMER_H
#define __SYS_TIMER_H

#define CORE0_TIMER_IRQ_CTRL ( (volatile unsigned int *) 0x40000040 )
#define CORE1_TIMER_IRQ_CTRL ( (volatile unsigned int *) 0x40000044 )
#define CORE2_TIMER_IRQ_CTRL ( (volatile unsigned int *) 0x40000048 )
#define CORE3_TIMER_IRQ_CTRL ( (volatile unsigned int *) 0x4000004C )

#define LOCAL_TIMER_CTRL ( (volatile unsigned int *) 0x40000034 )
#define LOCAL_TIMER_CLR  ( (volatile unsigned int *) 0x40000038 )

void sys_core_timer_enable ( );
void sys_core_timer_disable ( );
void sys_core_timer_reload ( );

void sys_local_timer_enable ( );
void sys_local_timer_disable ( );
void sys_local_timer_reload ( );

#endif