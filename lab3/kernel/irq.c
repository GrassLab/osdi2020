#include "irq.h"
#include "timer.h"
#include "uart.h"

void enable_interrupt_controller() { *(ENABLE_IRQS_1) = SYSTEM_TIMER_IRQ_1; }

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
      break;                                                                   \
    }                                                                          \
  }

void irq_handler() {
  unsigned int irq0 = *CORE0_IRQ_SRC;

  do {
    IRQ_CHECK(irq0, 0x02, {
      core_timer_handler();
      uart_println("Core timer interrupt, jiffies %d", ccnt++);
    });

    IRQ_CHECK(irq0, 0x800, {
      local_timer_handler();
      uart_println("Local timer interrupt, jiffies %d", lcnt++);
    });

    uart_println("Unknown pending irq: %x", irq0);

  } while (0);

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
