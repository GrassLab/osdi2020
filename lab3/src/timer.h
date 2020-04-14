#ifndef TIMER
#define TIMER

#define SYSTEM_TIMER_COMPARE1 0x3f003010
#define SYSTEM_TIMER_CLO 0x3f003004
#define SYSTEM_TIMER_CS 0x3f003000
#define IRQ_ENABLE0 0x3f00b210

#define ARM_TIMER_LOAD 0x3f00b400
#define ARM_TIMER_CONTROL 0x3f00b408
#define ARM_TIMER_IRQ_CLR 0x3f00b40c
#define IRQ_ENABLE2 0x3f00b218

#define LOCAL_TIMER_CONTROL_REG (unsigned int *)0x40000034
#define LOCAL_TIMER_IRQ_CLR (unsigned int *)0x40000038

extern void core_timer_enable();
extern void core_timer_disable();
extern void core_timer_handler();
void setRegister(unsigned int address, unsigned int value);
unsigned int getRegister(unsigned int address);
void sys_timer_init();
void sys_timer_handler();
void arm_timer_init();
void arm_timer_hanler();
void local_timer_init();
void local_timer_handler();

#endif