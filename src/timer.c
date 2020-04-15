#include "timer.h"
// #include "printf.h"

#define TIME_INTERVAL 2500000

// void sys_timer_init ()
// {
//     unsigned int curVal;
// 	curVal = *SYSTEM_TIMER_CLO;
//     *SYSTEM_TIMER_CMP1 = curVal + TIME_INTERVAL;
// }

// void sys_timer_handler(){
//     unsigned int curVal;
//     curVal = *SYSTEM_TIMER_CLO;
//     *SYSTEM_TIMER_CMP1 = curVal + TIME_INTERVAL;
//     *SYSTEM_TIMER_CS = 0x20; // To clear the interrupt//https://embedded-xinu.readthedocs.io/en/latest/arm/rpi/BCM2835-System-Timer.html
//     // *SYSTEM_TIMER_CS = 0xF;
// 	printf("Systeim timer interrupt received\n");
// }


/* local timer */
void local_timer_enable(){
    *LOCAL_TIMER_CTRL_REG = (LOCAL_TIMER_CTRL_FLAG | TIME_INTERVAL);
}

void local_timer_disable(){
    *LOCAL_TIMER_CTRL_REG &= ~(LOCAL_TIMER_CTRL_FLAG);
}


void local_timer_handler(){
    *LOCAL_TIMER_IRQ_CLR_REG = 0xc0000000; // clear interrupt and reload.
}