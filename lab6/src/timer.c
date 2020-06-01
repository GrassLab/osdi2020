
#include "io.h"
#include "mm.h"
#include "irq.h"
#include "task.h"
#include "sched.h"

#define LOCAL_TIMER_CONTROL_REG ((volatile unsigned int *)(0x40000034))
#define LOCAL_TIMER_IRQ_CLR ((volatile unsigned int *)(0x40000038))

void local_timer_init() {
  puts("Init local timer, done");
  unsigned int flag = 0x30000000; // enable timer and interrupt.
  unsigned int reload = 25000000;
  *(LOCAL_TIMER_CONTROL_REG) = flag | reload;
}

void local_timer_handler() {
  static unsigned int jiffies = 0;
  *(LOCAL_TIMER_IRQ_CLR) = 0xc0000000; // clear interrupt and reload.
  printf("Local timer interrupt, %d" NEWLINE, jiffies++);
}

#define SYSTEM_TIMER_COMPARE1 ((volatile unsigned int *)(0x3f003010))
#define SYSTEM_TIMER_CLO ((volatile unsigned int *)(0x3f003004))
#define SYSTEM_TIMER_CS ((volatile unsigned int *)(0x3f003000))

void sys_timer_init() {
  puts("Init system timer, done");
  unsigned int t;
  t = *SYSTEM_TIMER_CLO;
  *(SYSTEM_TIMER_COMPARE1) = t + 2500000;
  *(ENABLE_IRQS_1) = SYSTEM_TIMER_IRQ_1;
}

void sys_timer_handler() {
  unsigned int t;
  static unsigned int jiffies = 0;
  t = *SYSTEM_TIMER_CLO;
  *(SYSTEM_TIMER_COMPARE1) = t + 2500000;
  *(SYSTEM_TIMER_CS) = 0xf;
  printf("System timer interrupt, %d" NEWLINE, jiffies++);
}

#if MLV <= 2
#define CORE0_TIMER_IRQ_CTRL "0xffff000040000040"
//#define CORE0_TIMER_IRQ_CTRL "0x40000040"
#else
#define CORE0_TIMER_IRQ_CTRL "0x40000040"
#endif

void core_timer_init() {
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
//__asm__ volatile("mov x0, " EXPIRE_PERIOD);
#define RESCHED_INTERVAL 3
void core_timer_handler() {
  static unsigned int jiffies = 0;
  __asm__ volatile("mrs x0, cntfrq_el0");
  __asm__ volatile("msr cntp_tval_el0, x0");
  //printf("Core timer interrupt, %d" NEWLINE, jiffies++);
  if(jiffies % RESCHED_INTERVAL == 0 && current_task){
    current_task->flag |= RESCHED;
  }
  //puts("======== timer runing  =========");
  if(!current_task->preempt_count){
    unsigned long elr, sp_el0, spsr_el1;
    __asm__ volatile("mrs %0, elr_el1" : "=r"(elr));
    __asm__ volatile("mrs %0, sp_el0" : "=r"(sp_el0));
    __asm__ volatile("mrs %0, spsr_el1" : "=r"(spsr_el1));
    enable_irq();
    if(preempt_reschedable()) schedule();
    disable_irq();
    __asm__ volatile("msr elr_el1, %0" ::"r"(elr));
    __asm__ volatile("msr sp_el0, %0" ::"r"(sp_el0));
    __asm__ volatile("msr spsr_el1, %0" ::"r"(spsr_el1));
  }
  else{
    __asm__ volatile("mov x0, sp");
    __asm__ volatile("bl show_addr");
    puts("disable preempt");
    printf("preempt_count %d" NEWLINE, current_task->preempt_count);
  }
}
