#ifndef	_TIMER_H
#define	_TIMER_H

// #include "base.h"

// #define SYSTEM_TIMER_CS        ((volatile unsigned int*)(SYS_TIM_REG_BASE+0x00)) //System Timer Control and Status
// #define SYSTEM_TIMER_CLO       ((volatile unsigned int*)(SYS_TIM_REG_BASE+0x04)) //System Timer Counter Lower 32 bits
// #define SYSTEM_TIMER_CHI       ((volatile unsigned int*)(SYS_TIM_REG_BASE+0x08)) //System Timer Counter Upper 32 bits
// #define SYSTEM_TIMER_CMP0      ((volatile unsigned int*)(SYS_TIM_REG_BASE+0x0C)) //System Timer Compare 0; corresponds to IRQ line 0
// #define SYSTEM_TIMER_CMP1      ((volatile unsigned int*)(SYS_TIM_REG_BASE+0x10)) //System Timer Compare 1; corresponds to IRQ line 1
// #define SYSTEM_TIMER_CMP2      ((volatile unsigned int*)(SYS_TIM_REG_BASE+0x14)) //System Timer Compare 2; corresponds to IRQ line 2
// #define SYSTEM_TIMER_CMP3      ((volatile unsigned int*)(SYS_TIM_REG_BASE+0x18)) //System Timer Compare 3; corresponds to IRQ line 3

// #define TIMER_CS_M0	(1 << 0)
// #define TIMER_CS_M1	(1 << 1)
// #define TIMER_CS_M2	(1 << 2)
// #define TIMER_CS_M3	(1 << 3)

/*
 * local timer
 */
#define LOCAL_TIMER_CTRL_REG                   ((volatile unsigned int*)0x40000034)
#define LOCAL_TIMER_IRQ_CLR_REG                ((volatile unsigned int*)0x40000038)
#define LOCAL_TIMER_CTRL_INTEN                 (1<<29) //interrupt enable
#define LOCAL_TIMER_CTRL_TIMEN                 (1<<28) //timer enable
#define LOCAL_TIMER_CTRL_RELOAD_MASK           (0xFFFFFFF)
#define LOCAL_TIMER_CLR_CLEAR                  (1<<31)
#define LOCAL_TIMER_CLR_RELOAD                 (1<<30)

#define LOCAL_TIMER_CTRL_FLAG                  (LOCAL_TIMER_CTRL_TIMEN | LOCAL_TIMER_CTRL_INTEN)

// void sys_timer_init();
// void sys_timer_handler();
void core_timer_enable();
void core_timer_disable();
void core_timer_handler();
void local_timer_enable();
void local_timer_disable();
void local_timer_handler();

#endif  /*_TIMER_H */