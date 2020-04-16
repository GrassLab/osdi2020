#include "exception.h"
#include "types.h"
#include "shell.h"

void exception_init(void) {
  asm("msr vbar_el1, %0" : : "r"(vector_table));

  // Enable interrupt
  asm("msr daifclr, #0xf");
}

void core_timer_enable(void) {
  // Enable timer
  asm("mov x0, 1");
  asm("msr cntp_ctl_el0, x0");

  // Set the interval to be approximately 1 second
  asm("mrs x0, cntfrq_el0");
  asm("msr cntp_tval_el0, x0");

  *CORE0_TIMER_IRQ_CTRL = 2;
}

void system_timer_enable(void) {
  // Set the interval to be approximately 3 seconds
  *SYSTEM_TIMER_C1 = *SYSTEM_TIMER_CL0 + 3 * SYSTEM_TIMER_FREQUENCY;
  *IRQ_ENABLE1 = 1 << 1;
}

void curr_el_spx_sync_handler(void) {
  uint64_t address, syndrome;
  asm("mrs %0, elr_el1" : "=r"(address));
  asm("mrs %0, esr_el1" : "=r"(syndrome));

  // EC, bits [31:26]
  // ISS, bits [24:0]
  uint64_t ec = (syndrome & 0xfc000000) >> 26;
  uint64_t iss = syndrome & 0x01ffffff;

  char buf[32];
  if (ec == 0x15) {
    switch (iss) {
      case 1:
        mini_uart_puts("Exception return address 0x");
        mini_uart_puts(uitos_generic(address, 16, buf));
        mini_uart_puts(EOL);
        mini_uart_puts("Exception class (EC) 0x");
        mini_uart_puts(uitos_generic(ec, 16, buf));
        mini_uart_puts(EOL);
        mini_uart_puts("Instruction specific syndrome (ISS) 0x");
        mini_uart_puts(uitos_generic(iss, 16, buf));
        mini_uart_puts(EOL);
        break;
      case 2:
        core_timer_enable();
        system_timer_enable();
        break;
    }
  }
}

void curr_el_spx_irq_handler(void) {
//  char buf[32];
//  mini_uart_puts("basic pending: ");
//  mini_uart_puts(uitos(*IRQ_BASIC_PENDING, buf));
//  mini_uart_puts(EOL);
//  mini_uart_puts("pending 1: ");
//  mini_uart_puts(uitos(*IRQ_PENDING1, buf));
//  mini_uart_puts(EOL);
//  mini_uart_puts("pending 2: ");
//  mini_uart_puts(uitos(*IRQ_PENDING2, buf));
//  mini_uart_puts(EOL);
//  mini_uart_puts("core 0 interrupt source: ");
//  mini_uart_puts(uitos(*CORE0_INTERRUPT_SRC, buf));
//  mini_uart_puts(EOL);

  uint32_t irq_src = *CORE0_INTERRUPT_SRC;
  while (irq_src != 0) {
    uint8_t src = 31 - __builtin_clz(irq_src);

    switch (src) {
      case 1:
        core_timer_handler();
        break;
      case 8:
        gpu_interrupt_handler();
        break;
    }

    irq_src &= ~(1 << src);
  }
}

static uint64_t core_timer_jiffie = 0;
static uint64_t system_timer_jiffie = 0;

void core_timer_handler(void) {
  char buf[32];
  mini_uart_puts("Core timer interrupt, jiffies ");
  mini_uart_puts(uitos(++core_timer_jiffie, buf));
  mini_uart_puts(EOL);

  // Set the interval to be approximately 1 second
  asm("mrs x0, cntfrq_el0");
  asm("msr cntp_tval_el0, x0");
}

void gpu_interrupt_handler(void) {
  uint32_t irq_src = *IRQ_PENDING1;
  while (irq_src != 0) {
    uint8_t src = 31 - __builtin_clz(irq_src);

    switch (src) {
      case 1:
        system_timer_handler();
        break;
    }

    irq_src &= ~(1 << src);
  }
}

void system_timer_handler(void) {
  char buf[32];
  mini_uart_puts("System timer interrupt, jiffies ");
  mini_uart_puts(uitos(++system_timer_jiffie, buf));
  mini_uart_puts(EOL);

  // Set the interval to be approximately 3 seconds
  *SYSTEM_TIMER_C1 = *SYSTEM_TIMER_CL0 + 3 * SYSTEM_TIMER_FREQUENCY;
  // Clear the match detect status bit and the corresponding interrupt request line.
  *SYSTEM_TIMER_CS = 0xf;
}

void not_implemented_handler(void) {
  mini_uart_puts("This handler is not implemented yet" EOL);
}
