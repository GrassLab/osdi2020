#include "include/utils.h" 

#define SYSTEM_TIMER_COMPARE1 0x3f003010
#define SYSTEM_TIMER_CLO 0x3f003004
#define SYSTEM_TIMER_CS 0x3f003000
#define IRQ_ENABLE0 0x3f00b210

void sys_timer_init(){
  unsigned int t;
  t = get32(SYSTEM_TIMER_CLO);
  put32(SYSTEM_TIMER_COMPARE1, t + 2500000);
  put32(IRQ_ENABLE0, 1 << 1);
}

void sys_timer_handler(){
  unsigned int t;
  t = get32(SYSTEM_TIMER_CLO);
  put32(SYSTEM_TIMER_COMPARE1, t + 2500000);
  put32(SYSTEM_TIMER_CS, 0xf);
}
