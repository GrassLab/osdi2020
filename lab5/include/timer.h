#ifndef _TIMER_H
#define _TIMER_H

#define PBASE 0x3F000000
#define TIMER_CS        (PBASE+0x00003000)
#define TIMER_CLO       (PBASE+0x00003004)
#define TIMER_CHI       (PBASE+0x00003008)
#define TIMER_C0        (PBASE+0x0000300C)
#define TIMER_C1        (PBASE+0x00003010)
#define TIMER_C2        (PBASE+0x00003014)
#define TIMER_C3        (PBASE+0x00003018)

#define TIMER_CS_M0	(1 << 0)
#define TIMER_CS_M1	(1 << 1)
#define TIMER_CS_M2	(1 << 2)
#define TIMER_CS_M3	(1 << 3)

#define LOCAL_TIMER_CONTROL_REG 0x40000034
#define LOCAL_TIMER_IRQ_CLR     0x40000038

#define CORE0_TIMER_IRQ_CTRL 0x40000040
#define CORE0_EXPIRE_PERIOD 0xfffffff

void timer_init ( void );
void handle_timer_irq( void );
void core_timer_init();
void core_timer_handler();
void local_timer_init();
void local_timer_handler();

#endif
