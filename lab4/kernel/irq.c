#include "irq.h"
#include "libc.h"
#include "miniuart.h"
#include "timer.h"
#include "bh.h"

void enable_interrupt_controller() { *(ENABLE_IRQS_1) = AUX_IRQ; }

unsigned long timer_read_counter(void) {
  unsigned long cntpct;
  asm volatile("isb");
  asm volatile("mrs %0, cntpct_el0" : "=r"(cntpct));

  return cntpct;
}

unsigned long ccnt = 1;
unsigned long lcnt = 1;

#define IRQ_CHECK(input, and_bit, doing)                                       \
  {                                                                            \
    if ((input) & (and_bit)) {                                                 \
      {                                                                        \
        doing                                                                  \
      }                                                                        \
      flag = 0;                                                                \
    }                                                                          \
  }


void irq_handler() {
  unsigned int irq0 = *CORE0_IRQ_SRC;
  unsigned int irqp = *IRQ_PENDING_1;
  int flag = 1;

  do {
    flag = 1;

    IRQ_CHECK(irq0, 0x02, {
      if (bh_mod_mask == 1) {
        bh_core_timer_handler();
        /* extract a job from bh_base */
        bh_handler job = extract_bh_handler();
        if (job) {
          uart_println("\e[0;31m[!] Bh Core timer interrupt, doing a job\e[0m");
          job();
        } else {
          uart_println("\e[0;32m[?] Bh Core timer interrupt, without job\e[0m");
        }
      } else {
        core_timer_handler();
        uart_println("Core timer interrupt, jiffies %d", ccnt++);
      }
    });

    IRQ_CHECK(irq0, 0x800, {
      if (bh_mod_mask == 1) {
        push_bh_handle(local_timer_display_handler, 0);
        clear_local_timer_interrupt();
      } else {
        local_timer_handler();
        /* uart_println("Local timer interrupt, jiffies %d", lcnt++); */
      }
    });

    IRQ_CHECK(irqp, AUX_IRQ, {
      uart_irq_handler();
      irqp &= ~AUX_IRQ;
    });

    if(flag) {
      uart_println("Unknown pending irq: %x", irqp);
      irqp = 0;
    }

  } while (irqp);

  /* unsigned int irq = *IRQ_PENDING_1; */
  /* switch (irq) { */
  /* case (SYSTEM_TIMER_IRQ_1): */
  /*   /\* handle_timer_irq(); *\/ */
  /*   break; */
  /* default: */
  /*   uart_println("Unknown pending irq: %x", irq); */
  /* } */

  /* core_timer_handler(); */
  /* uart_println("Core timer interrupt, jiffies %d", timer_read_counter()); */
}

void init_uart_irq() { *ENABLE_IRQS_2 = 1 << 25; }
