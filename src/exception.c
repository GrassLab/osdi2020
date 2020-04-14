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
        break;
    }
  }
}

static uint64_t jiffie = 0;

void curr_el_spx_irq_handler(void) {
  char buf[32];
  mini_uart_puts("Core timer interrupt, jiffies ");
  mini_uart_puts(uitos(++jiffie, buf));
  mini_uart_puts(EOL);

  // Set the interval to be approximately 1 second
  asm("mrs x0, cntfrq_el0");
  asm("msr cntp_tval_el0, x0");
}

void not_implemented_handler(void) {
  mini_uart_puts("This handler is not implemented yet" EOL);
}
