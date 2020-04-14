#include "timer.h"

void setRegister(unsigned int address, unsigned int value){
    *(volatile unsigned int*)(address) = value;
}

unsigned int getRegister(unsigned int address){
    return *(volatile unsigned int*)(address);
}

void sys_timer_init(){
  unsigned int t = getRegister(SYSTEM_TIMER_CLO);
  setRegister(SYSTEM_TIMER_COMPARE1, t + 2500000);
  setRegister(IRQ_ENABLE0, 1 << 1);
}

void sys_timer_handler(){
  unsigned int t = getRegister(SYSTEM_TIMER_CLO);
  setRegister(SYSTEM_TIMER_COMPARE1, t + 2500000);
  setRegister(SYSTEM_TIMER_CS, 0xf);
}

void arm_timer_init(){
  setRegister(ARM_TIMER_CONTROL, (1 << 7) | (1 << 5) | (1 << 1));
  setRegister(ARM_TIMER_LOAD, 500000);
  setRegister(IRQ_ENABLE2, 1);
}

void arm_timer_hanler(){
  setRegister(ARM_TIMER_IRQ_CLR, 1);
}

void local_timer_init(){
  unsigned int flag = 0x30000000; // enable timer and interrupt.
  unsigned int reload = 25000000;
  *LOCAL_TIMER_CONTROL_REG = flag | reload;
}

void local_timer_handler(){
  *LOCAL_TIMER_IRQ_CLR = 0xc0000000; // clear interrupt and reload.
}