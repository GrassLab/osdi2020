#include "timer.h"
#include "util.h"

// system timer
#define SYSTEM_TIMER_COMPARE1 0x3f003010
#define SYSTEM_TIMER_CLO 0x3f003004
#define SYSTEM_TIMER_CS 0x3f003000
#define IRQ_ENABLE0 0x3f00b210

void sys_timer_init(){
  unsigned int t;
  get32(SYSTEM_TIMER_CLO, &t);
  set32(SYSTEM_TIMER_COMPARE1, t + 2500000);
  set32(IRQ_ENABLE0, 1 << 1);
}

void sys_timer_handler(){
  unsigned int t;
  get32(SYSTEM_TIMER_CLO, &t);
  set32(SYSTEM_TIMER_COMPARE1, t + 2500000);
  set32(SYSTEM_TIMER_CS, 0xf);
}


// arm side timer
#define ARM_TIMER_LOAD 0x3f00b400
#define ARM_TIMER_CONTROL 0x3f00b408
#define ARM_TIMER_IRQ_CLR 0x3f00b40c
#define IRQ_ENABLE2 0x3f00b218

void arm_timer_init(){
  set32(ARM_TIMER_CONTROL, (1 << 7) | (1 << 5) | (1 << 1));
  set32(ARM_TIMER_LOAD, 500000);
  set32(IRQ_ENABLE2, 1);
}

void arm_timer_hanler(){
  set32(ARM_TIMER_IRQ_CLR, 1);
}


// arm local timer
#define LOCAL_TIMER_CONTROL_REG 0x40000034

void local_timer_init(){
  unsigned int flag = 0x30000000; // enable timer and interrupt.
  unsigned int reload = 25000000;
  set32(LOCAL_TIMER_CONTROL_REG, flag | reload);
}

#define LOCAL_TIMER_IRQ_CLR 0x40000038

void local_timer_handler(){
  set32(LOCAL_TIMER_IRQ_CLR, 0xc0000000); // clear interrupt and reload.
}

// arm core timer
// in timer.S
