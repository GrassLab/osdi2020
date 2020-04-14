
#include "io.h"
#define LOCAL_TIMER_CONTROL_REG ((volatile unsigned int*)(0x40000034))
#define LOCAL_TIMER_IRQ_CLR     ((volatile unsigned int*)(0x40000038))

void local_timer_init(){
    puts("Init local timer, done");
    unsigned int flag = 0x30000000;// enable timer and interrupt.
    unsigned int reload = 25000000;
    *(LOCAL_TIMER_CONTROL_REG) = flag | reload;
}

void local_timer_handler(){
    static unsigned int jiffies = 0;
    *(LOCAL_TIMER_IRQ_CLR) = 0xc0000000; // clear interrupt and reload.
    printf("Local timer interrupt, %d" NEWLINE, jiffies++);
}

#define SYSTEM_TIMER_COMPARE1  ((volatile unsigned int*)(0x3f003010))
#define SYSTEM_TIMER_CLO       ((volatile unsigned int*)(0x3f003004))
#define SYSTEM_TIMER_CS        ((volatile unsigned int*)(0x3f003000))
#define IRQ_ENABLE0            ((volatile unsigned int*)(0x3f00b210))

void sys_timer_init(){
    puts("Init system timer, done");
    unsigned int t;
    t = *SYSTEM_TIMER_CLO;
    *(SYSTEM_TIMER_COMPARE1) = t + 2500000;
    *(IRQ_ENABLE0) = 1 << 1;
}

void sys_timer_handler(){
    unsigned int t;
    static unsigned int jiffies = 0;
    t = *SYSTEM_TIMER_CLO;
    *(SYSTEM_TIMER_COMPARE1) = t + 2500000;
    *(SYSTEM_TIMER_CS) = 0xf;
    printf("System timer interrupt, %d" NEWLINE, jiffies++);
}

#define CORE0_TIMER_IRQ_CTRL "0x40000040"

void core_timer_init(){
#if defined(RUN_ON_EL1) || defined(RUN_ON_EL2)
    puts("Init core timer, done");
    __asm__ volatile("mov x0, 1");
    __asm__ volatile("msr cntp_ctl_el0, x0"); // enable timer
    __asm__ volatile("mov x0, 2");
    __asm__ volatile("ldr x1, =" CORE0_TIMER_IRQ_CTRL);
    __asm__ volatile("str x0, [x1]"); // enable timer interrupt
    __asm__ volatile("mrs x0, cntfrq_el0");
    __asm__ volatile("msr cntp_tval_el0, x0");
#else
    __asm__ volatile("stp x8, x9, [sp, #-16]!");
    __asm__ volatile("mov x8, #0");
    __asm__ volatile("svc #0");
    __asm__ volatile("ldp x8, x9, [sp], #16");
#endif
}

#define EXPIRE_PERIOD "0x1ffffff"
void core_timer_handler(){
    static unsigned int jiffies = 0;
    //__asm__ volatile("mov x0, " EXPIRE_PERIOD);
    __asm__ volatile("mrs x0, cntfrq_el0");
    __asm__ volatile("msr cntp_tval_el0, x0");
    printf("Core timer interrupt, %d" NEWLINE, jiffies++);
}
