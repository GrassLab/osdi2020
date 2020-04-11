#include "exception.h"
#include "types.h"
#include "shell.h"

void exception_init(void) {
  asm("msr vbar_el2, %0" : : "r"(vector_table));
}

void curr_el_spx_sync_handler(void) {
  uint64_t address, syndrome;
  asm("mrs %0, elr_el2" : "=r"(address));
  asm("mrs %0, esr_el2" : "=r"(syndrome));

  // EC, bits [31:26]
  // ISS, bits [24:0]
  uint64_t ec = (syndrome & 0xfc000000) >> 26;
  uint64_t iss = syndrome & 0x01ffffff;

  char buf[32];
  mini_uart_puts("Exception return address 0x");
  mini_uart_puts(uitos_generic(address, 16, buf));
  mini_uart_puts(EOL);
  mini_uart_puts("Exception class (EC) 0x");
  mini_uart_puts(uitos_generic(ec, 16, buf));
  mini_uart_puts(EOL);
  mini_uart_puts("Instruction specific syndrome (ISS) 0x");
  mini_uart_puts(uitos_generic(iss, 16, buf));
  mini_uart_puts(EOL);
}

void not_implemented(void) {
  mini_uart_puts("This handler is not implemented yet" EOL);
}
