#include "irq.h"
#include "mm.h"
#include "info.h"
#include "io.h"
#include "shell.h"
#include "timer.h"
#include "uart.h"
#include "util.h"
#include "task.h"
#include "sched.h"

#define nb8p(bytes, n, p) (((1 << n) - 1) & (bytes >> (p)))

#define task_size 32

unsigned int task_ptr = 0;
void (*task_queue[task_size])(void);

void push_deffered(void (*task)(void)) { task_queue[task_ptr++] = task; }

void (*pop_deffered(void))() { return task_queue[--task_ptr]; }

void bottom_half(void) {
  // puts("doing bottom_half...");
  delay(5000000);
  puts(NEWLINE "isr rest done.");
}

Task *read_tasks[TASK_SIZE];
int rtbeg = 0, rtend = 0;

extern Task *current_task;
void uart_read_enqueue(Task *task){
  read_tasks[rtend] = task;
  rtend = (rtend + 1) % TASK_SIZE;
  task->status = block;
}

int uart_read_front(){
  int s = rtbeg;
  for(int i = rtbeg + 1; i < rtend; i++){
    if(read_tasks[s]->priority < read_tasks[i]->priority){
      s = i;
    }
  }
  return s;
}

void uart_read_dequeue(int s){
  if(s == rtbeg) rtbeg++;
  else{
    for(int i = s; i < rtend - 1; i++){
      read_tasks[i] = read_tasks[i + 1];
    }
    rtend--;
  }
}

void handle_uart_irq(void) {
  // There may be more than one byte in the FIFO.
  while ((*(AUX_MU_IIR_REG)&IIR_REG_REC_NON_EMPTY) == IIR_REG_REC_NON_EMPTY) {
    if(rtbeg == rtend){
      uart_recv();
    }
    else{
#if 1
      int p = uart_read_front();
      read_tasks[p]->buffer[0] = uart_recv();
      read_tasks[p]->status = idle;
      read_tasks[p]->counter += 100;
      uart_read_dequeue(p);
#else
      read_tasks[rtbeg]->buffer[0] = uart_recv();
      read_tasks[rtbeg]->status = idle;
      read_tasks[rtbeg]->priority = 10;
      rtbeg = (rtbeg + 1) % TASK_SIZE;
#endif
      current_task->flag |= RESCHED;
      unsigned long elr, sp_el0, spsr_el1;
      __asm__ volatile("mrs %0, elr_el1" : "=r"(elr));
      __asm__ volatile("mrs %0, sp_el0" : "=r"(sp_el0));
      __asm__ volatile("mrs %0, spsr_el1" : "=r"(spsr_el1));
      enable_irq();
      schedule();
      disable_irq();
      __asm__ volatile("msr elr_el1, %0" ::"r"(elr));
      __asm__ volatile("msr sp_el0, %0" ::"r"(sp_el0));
      __asm__ volatile("msr spsr_el1, %0" ::"r"(spsr_el1));
    }
  }
}

#if MLV < 2
#define LOCAL_TIMER_CONTROL_REG ((volatile unsigned int *)(VA_START + 0x40000034))
#define CORE0_INT_SRC ((volatile unsigned int *)(VA_START + 0x40000060))
#else
#define LOCAL_TIMER_CONTROL_REG ((volatile unsigned int *)(0x40000034))
#define CORE0_INT_SRC ((volatile unsigned int *)(0x40000060))
#endif

#define miniUART_IRQ (1 << 0)
void irq_handler() {

  unsigned int irq = *(IRQ_PENDING_1);
  // unsigned int aux_irq = *(AUX_IRQ);
  //*DISABLE_IRQS_1 = (SYSTEM_TIMER_IRQ_1 | AUX_IRQ_MSK);

  if (nb8p(*LOCAL_TIMER_CONTROL_REG, 1, 31))
    local_timer_handler();
  if((*CORE0_INT_SRC) & 0x2){
    core_timer_handler();
  }

  while (irq) {
    if (irq & SYSTEM_TIMER_IRQ_1) {
      sys_timer_handler();
      irq &= ~SYSTEM_TIMER_IRQ_1;
    } else if (irq & AUX_IRQ_MSK) {
      handle_uart_irq();
      irq &= ~AUX_IRQ_MSK;
    } else {
      printf("Unknown pending irq: %x" NEWLINE, irq);
      irq = 0;
    }
  }

  unsigned long elr, nelr;
  __asm__ volatile("mrs %0, elr_el1" : "=r"(elr));

#ifdef BOTTOM_HALF
#ifdef DEFFERED
  push_deffered(bottom_half);
  enable_irq();
#else
  bottom_half();
#endif
#endif

  __asm__ volatile("mrs %0, elr_el1" : "=r"(nelr));
  if (elr != nelr) {
    __asm__ volatile("msr elr_el1, %0" ::"r"(elr));
  }
}

void init_irq() {

#if defined(RUN_ON_EL2)
  unsigned long hcr;
  __asm__ volatile("mrs %0, hcr_el2" : "=r"(hcr));
  hcr |= 1 << 4; // IMO
  __asm__ volatile("msr hcr_el2, %0" ::"r"(hcr));
#endif
  //__asm__ volatile ("msr  daifclr, #2");
  *ENABLE_IRQS_1 = (SYSTEM_TIMER_IRQ_1 | AUX_IRQ_MSK);
  /* disable irq */
  __asm__ volatile("msr daifclr, #2" ::: "memory");
}

void enable_irq() {
  __asm__ volatile("msr daifclr, #2" ::: "memory");
}

void disable_irq() {
  __asm__ volatile("msr daifset, #2" ::: "memory");
}
