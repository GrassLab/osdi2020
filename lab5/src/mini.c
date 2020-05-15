
#ifdef MINIUART
#include "gpio.h"
#include "uart.h"
#include "util.h"

void uart_send(unsigned int c) {
  while (!(*(AUX_MU_LSR_REG)&0x20))
    __asm__ volatile("nop");
  *(AUX_MU_IO_REG) = c;
}

char uart_recv(void) {
  while (!(*(AUX_MU_LSR_REG)&0x01))
    __asm__ volatile("nop");
  return *(AUX_MU_IO_REG)&0xFF;
}

void uart_flush(void) {
  while (*(AUX_MU_LSR_REG)&0x01)
    *(AUX_MU_IO_REG);
}

void uart_puts(char *str) {
  while (*str)
    uart_send((char)*str), str++;
}

void uart_init(void) {
  unsigned int reg;
  reg = *(GPFSEL1);
  reg &= ~(7 << 12);
  reg |= 2 << 12;
  reg &= ~(7 << 15);
  reg |= 2 << 15;
  *(GPFSEL1) = reg;

  *(GPPUD) = 0;
  DELAY(150);
  *(GPPUDCLK0) = (1 << 14) | (1 << 15);
  DELAY(150);
  *(GPPUDCLK0) = 0;
  *(AUX_ENABLES) = 1;
  *(AUX_MU_CNTL_REG) = 0;

#ifdef UARTINT
  *(AUX_MU_IER_REG) = IER_REG_VALUE; // Enable receive interrupts
#else
  *(AUX_MU_IER_REG) = 0;
#endif

  *(AUX_MU_LCR_REG) = 3;
  *(AUX_MU_MCR_REG) = 0;
  *(AUX_MU_BAUD_REG) = 270;
  *(AUX_MU_CNTL_REG) = 3;
}

#endif
