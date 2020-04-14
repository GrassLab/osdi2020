#include "timer.h"
// #include "printf.h"

// #define TIME_INTERVAL 2500000

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
#define LOCAL_TIMER_CONTROL_REG ((volatile unsigned int*)0x40000034)

void local_timer_init(){
    unsigned int flag = 0x30000000; // enable timer and interrupt.
    unsigned int reload = 25000000;
    *LOCAL_TIMER_CONTROL_REG = (flag|reload);
}

#define LOCAL_TIMER_IRQ_CLR ((volatile unsigned int*)0x40000038)

void local_timer_handler(){
    *LOCAL_TIMER_IRQ_CLR = 0xc0000000; // clear interrupt and reload.
}