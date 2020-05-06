#include "lock.h"
#include "mini_uart.h"

void gpio_init(void) {
  // Set alternative function for GPIO pins.
  // GPIO14 ALT5: TXD1 => Set GPFSEL1[12:14] to 010
  // GPIO15 ALT5: RXD1 => Set GPFSEL1[15:17] to 010
  uint32_t sel = *GPFSEL1;
  sel &= ~(7 << 12);
  sel |= (1 << 13);
  sel &= ~(7 << 15);
  sel |= (1 << 16);
  *GPFSEL1 = sel;

  // Disable GPIO pull up/down
  *GPPUD = 0;
  // Wait 150 cycles – this provides the required set-up time for the control signal.
  for (int i = 0; i < 150; ++i);
  // Clock the control signal into the GPIO pins 14 and 15.
  *GPPUDCLK0 |= ((1 << 14) | (1 << 15));
  // Wait 150 cycles – this provides the required hold time for the control signal.
  for (int i = 0; i < 150; ++i);
  // Remove the control signal.
  *GPPUD = 0;
  // Remove the clock.
  *GPPUDCLK0 = 0;

  mutex_init(&uart_lock);
}

void mini_uart_init(void) {
  // Set AUXENB register to enable mini UART. Then mini UART register can be accessed.
  *AUXENB = 1;
  // Disable transmitter and receiver during configuration.
  *AUX_MU_CNTL_REG = 0;
  // Disable interrupt because currently you don’t need interrupt.
  *AUX_MU_IER_REG = 0;
  // Set the data size to 8 bit.
  *AUX_MU_LCR_REG = 3;
  // Don’t need auto flow control.
  *AUX_MU_MCR_REG = 0;
  // Set baud rate to 115200.
  *AUX_MU_BAUD = 270;
  // No FIFO.
  *AUX_MU_IIR_REG = 6;
  // Enable the transmitter and receiver.
  *AUX_MU_CNTL_REG = 3;
}

uint8_t mini_uart_getc(bool verbose) {
  while ((*AUX_MU_LSR_REG & 1) == 0) {}
  uint8_t c = *AUX_MU_IO_REG & 0xff;
  if (verbose) {
    mini_uart_putc(c);
  }
  return c;
}

void mini_uart_getn(bool verbose, uint8_t *buf, uint32_t n) {
  for (uint32_t i = 0; i < n; ++i) {
    buf[i] = mini_uart_getc(verbose);
  }
}

void mini_uart_gets(char *buf) {
  char c;
  while (c = mini_uart_getc(true), c != '\r' && c != '\n') {
    *buf = c;
    ++buf;
  }
  *buf = '\0';
}

void mini_uart_putc(uint8_t c) {
  while ((*AUX_MU_LSR_REG & (1 << 5)) == 0) {}
  *AUX_MU_IO_REG = c;
}

void mini_uart_putn(const uint8_t *buf, uint32_t n) {
  for (uint32_t i = 0; i < n; ++i) {
    mini_uart_putc(buf[i]);
  }
}

void mini_uart_puts(const uint8_t *buf) {
  for (; *buf != '\0'; ++buf) {
    mini_uart_putc(*buf);
  }
}
