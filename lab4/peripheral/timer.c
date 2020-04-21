#include "timer.h"
#include "miniuart.h"
#include "libc.h"
#include "sched.h"

const unsigned int interval = 200000;
unsigned int curVal = 0;

void timer_init(void) {
  curVal = *TIMER_CLO;
  curVal += interval;
  *TIMER_C1 = curVal;
}

void handle_timer_irq() {
  curVal += interval;
  *TIMER_C1 =  curVal;
  *TIMER_CS =  TIMER_CS_M1;
  uart_println("Timer interrupt received");
}


void sys_print_time() {
  unsigned long long freq, cnt;

  /* get the current counter frequency / counter */
  asm volatile ("mrs %0, cntfrq_el0\n"
                "mrs %1, cntpct_el0\n" :
                "=r" (freq), "=r" (cnt));
  unsigned long long i_part = cnt / freq;
  unsigned long long f_part = cnt * 100000000 / freq % 100000000;

  uart_print("[%d.%d] ", i_part, f_part);
}

/* core timer */
void core_timer_enable() {
  asm volatile("stp x8, x9, [sp, #-16]!");
  asm volatile("mov x8, #0");   /* 0 for core timer */
  asm volatile("svc     #0");
  asm volatile("ldp x8, x9, [sp], #16");
}

void core_timer_enable_w4sec() {
  asm volatile("stp x8, x9, [sp, #-16]!");
  asm volatile("mov x8, #2");   /* 2 for core timer 2 sec*/
  asm volatile("svc     #0");
  asm volatile("ldp x8, x9, [sp], #16");
}

void sched_core_timer_handler() {
  unsigned long r;
  asm volatile ("mov %0, #0x300000" : "=r"(r));
  asm volatile ("msr cntp_tval_el0, %0" : "=r"(r));
  timer_tick();
}

/* local timer */
#define LOCAL_TIMER_CONTROL_REG ((volatile unsigned int *)0x40000034)

void local_timer_init(){
  unsigned int flag = 0x30000000; // enable timer and interrupt.
  unsigned int reload = 1200000;
  *LOCAL_TIMER_CONTROL_REG = flag | reload;
}

#define LOCAL_TIMER_IRQ_CLR     ((volatile unsigned int *)0x40000038)

void local_timer_handler(){
  *LOCAL_TIMER_IRQ_CLR = 0xc0000000; // clear interrupt and reload.
  timer_tick();
}

void clear_local_timer_interrupt() {
  *LOCAL_TIMER_IRQ_CLR = 0x80000000; // clear interrupt
}


void local_timer_display_handler() {
  static unsigned long lcnt = 1;
  *LOCAL_TIMER_IRQ_CLR = 0xc0000000; // clear interrupt and reload.
  uart_println("Local timer interrupt, jiffies %d", lcnt++);
}
