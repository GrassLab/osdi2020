#include "utils.h"
#include "printf.h"
#include "timer.h"
#include "entry.h"
#include "peripherals/irq.h"

const char *entry_error_messages[] = {
    "SYNC_INVALID_EL1t",
    "IRQ_INVALID_EL1t",
    "FIQ_INVALID_EL1t",
    "ERROR_INVALID_EL1T",

    "SYNC_INVALID_EL1h",
    "IRQ_INVALID_EL1h",
    "FIQ_INVALID_EL1h",
    "ERROR_INVALID_EL1h",

    "SYNC_INVALID_EL0_64",
    "IRQ_INVALID_EL0_64",
    "FIQ_INVALID_EL0_64",
    "ERROR_INVALID_EL0_64",

    "SYNC_INVALID_EL0_32",
    "IRQ_INVALID_EL0_32",
    "FIQ_INVALID_EL0_32",
    "ERROR_INVALID_EL0_32"
};

#define IRQ_CHECK(input, and_bit, doing)                                       \
  {                                                                            \
    if ((input) & (and_bit)) {                                                 \
      {doing} flag = 0;                                                        \
    }                                                                          \
  }


void enable_interrupt_controller()
{
    /* put32(ENABLE_IRQS_1, SYSTEM_TIMER_IRQ_1); */
}

void show_invalid_entry_message(int type, unsigned long esr, unsigned long address)
{
    printf("%s, ESR: %x, address: %x\r\n", entry_error_messages[type], esr, address);
}

void handle_irq(void) {
  unsigned int irq0 = *CORE0_IRQ_SRC;
  unsigned int irqp = get32(IRQ_PENDING_1);

  /* unsigned int irq = get32(IRQ_PENDING_1); */

  int flag = 1;

  do {
    flag = 1;
    IRQ_CHECK(irq0, 0x02, {
        sched_core_timer_handler();
    });
    if(flag) {
      printf("Unknown pending irq: %x", irqp);
      irqp = 0;
    }
  } while (irqp);


  /* switch (irq) { */
  /* case (SYSTEM_TIMER_IRQ_1): */
  /*   handle_timer_irq(); */
  /*   break; */
  /* default: */
  /*   printf("Inknown pending irq: %x\r\n", irq); */
  /* } */
}
