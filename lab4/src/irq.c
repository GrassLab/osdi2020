#include "irq.h"
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
#ifdef ON_QEMU
  delay(500000000);
#else
  delay(5000000);
#endif
  puts(NEWLINE "isr rest done.");
}

Task *read_tasks[TASK_SIZE];
int rtbeg = 0, rtend = 0;

extern Task *current_task;
void uart_read_enqueue(Task *task){
  read_tasks[rtend] = task;
  rtend = (rtend + 1) % TASK_SIZE;
  task->status = block;
  printf("block is [%d](%d), %d %d",
      task->pid,
      task->status,
      task->priority, task->counter);
}

void handle_uart_irq(void) {
  // There may be more than one byte in the FIFO.
  while ((*(AUX_MU_IIR_REG)&IIR_REG_REC_NON_EMPTY) == IIR_REG_REC_NON_EMPTY) {
    if(rtbeg == rtend){
      shell_stuff_line(uart_recv());
      puts("<<<<<<<<<<<<<<<<<<<<<<<<< stuff shell");
    }
    else{
      read_tasks[rtbeg]->buffer[0] = uart_recv();
      read_tasks[rtbeg]->status = idle;
      rtbeg = (rtbeg + 1) % TASK_SIZE;
      puts(">>>>>>>>>>>>>>>>>>   stuff task");
      read_tasks[rtbeg]->priority = 10;
      current_task->flag |= RESCHED;
      enable_irq();
      schedule();
    }
  }
}

#define LOCAL_TIMER_CONTROL_REG ((volatile unsigned int *)(0x40000034))
#define CORE0_INT_SRC ((volatile unsigned int *)(0x40000060))

#define miniUART_IRQ (1 << 0)
void irq_handler() {
  unsigned int irq = *(IRQ_PENDING_1);
  // unsigned int aux_irq = *(AUX_IRQ);
  //*DISABLE_IRQS_1 = (SYSTEM_TIMER_IRQ_1 | AUX_IRQ_MSK);

  // get_current_el();

  if (nb8p(*LOCAL_TIMER_CONTROL_REG, 1, 31))
    local_timer_handler();
  if ((*CORE0_INT_SRC) & 0x2)
    core_timer_handler();

  while (irq) {
    if (irq & SYSTEM_TIMER_IRQ_1) {
      sys_timer_handler();
      irq &= ~SYSTEM_TIMER_IRQ_1;
    } else if (irq & AUX_IRQ_MSK) {
      // else if(aux_irq & miniUART_IRQ)
      handle_uart_irq();
      irq &= ~AUX_IRQ_MSK;
    } else {
      printf("Unknown pending irq: %x" NEWLINE, irq);
      irq = 0;
    }
  }

  unsigned long elr, nelr;
  __asm__ volatile("mrs %0, elr_el1" : "=r"(elr));
  // printf("pre elr = %x" NEWLINE, elr);

#ifdef BOTTOM_HALF
#ifdef DEFFERED
  push_deffered(bottom_half);
  enable_irq();
#else
  bottom_half();
#endif
#endif

  __asm__ volatile("mrs %0, elr_el1" : "=r"(nelr));
  // printf("%x vs %x" NEWLINE, elr, nelr);
  if (elr != nelr) {
    __asm__ volatile("msr elr_el1, %0" ::"r"(elr));
  }
  // delay(5000000000);
  // printf("b");
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
  // delay(50000000);
  //*ENABLE_IRQS_1 = (SYSTEM_TIMER_IRQ_1 | AUX_IRQ_MSK);
  __asm__ volatile("msr daifclr, #2" ::: "memory");
}

void disable_irq() {
  // get_current_el();
  // delay(500000000);
  //*DISABLE_IRQS_1 = (SYSTEM_TIMER_IRQ_1 | AUX_IRQ_MSK);
  __asm__ volatile("msr daifset, #2" ::: "memory");
}
