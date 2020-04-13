#ifndef TIMER_H
#define TIMER_H

#define SYSTEM_TIMER_COMPARE1 0x3f003010
#define SYSTEM_TIMER_CLO 0x3f003004
#define SYSTEM_TIMER_CS 0x3f003000
#define IRQ_ENABLE0 0x3f00b210

#define LOCAL_TIMER_CONTROL_REG 0x40000034
#define LOCAL_TIMER_IRQ_CLR     0x40000038

#define CORE0_TIMER_IRQ_CTRL 0x40000040
#define CORE0_EXPIRE_PERIOD 0xfffffff

#define SYSCALL_ENABLE_IRQ          0x100
#define SYSCALL_DISABLE_IRQ         0x101
#define SYSCALL_LOCAL_TIMER_INIT    0x102
#define SYSCALL_SYS_TIMER_INIT      0x103
#define SYSCALL_CORE_TIMER_INIT     0x104

void local_timer_init();
void __local_timer_init();
void local_timer_handler();
void sys_timer_init();
void __sys_timer_init();
void sys_timer_handler();
void core_timer_init();
void __core_timer_init();
void core_timer_handler();

#endif