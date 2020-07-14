#include "uart.h"
#include "mailbox.h"
#include "gpio.h"

void uart_init(void)
{
  /* 1. Disable UART */
  *UART_CR = 0;

  /* Setup UART clock rate to 3 mhz */
  mailbox_set_clock_rate(MAILBOX_UART_ID, 3000000);


  /* Setup GPIO */
  /* Configure GPFSELn register to change alternate function */
  /* GPIO 14 [14:12], 15 [17:15] belongs to GPFSEL1 */
  /* uart is ALT0 for GPIO 14, 15 */
  /* ALT0 = 100 = 4 */
  *GPFSEL1 &= (uint32_t)~(7 << 12); /* clear GPIO14 [14:12] */
  *GPFSEL1 &= (uint32_t)~(7 << 15); /* clear GPIO15 [17:15] */
  *GPFSEL1 |= (4 << 12); /* set GPIO14 to ALT0 */
  *GPFSEL1 |= (4 << 15); /* set GPIO14 to ALT0 */

  /* disable GPIO pull up/down */
  /* If you use a particular pin as input and don't connect anything to this pin,
   * you will not be able to identify whether the value of the pin is 1 or 0.
   * In fact, the device will report random values. The pull-up/pull-down mechanism
   * allows you to overcome this issue. If you set the pin to the pull-up state and
   * nothing is connected to it, it will report 1 all the time (for the pull-down state,
   * the value will always be 0). The pin state is preserved even after a reboot, so
   * before using any pin, we always have to initialize its state. */

  /* 1. Write to GPPUD to set the required control signal */
  *GPPUD = 0; /* 00 = Off - disable pull-up/down */

  /* 2. Wait 150 cycles - this provides the required set-up time for the control signal */
  for(int delay_count = 0; delay_count < 150; ++delay_count)
  {
    /* ++delay_count cost 1 cycle */
    /* technical speaking it may not be exact 150 cycles */
  }

  /* 3. Write to GPPUDCLK0/1 to clock the control signal into the GPIO pads you wish to
   * modify – NOTE only the pads which receive a clock will be modified, all others will
   * retain their previous state. */
  *GPPUDCLK0 = (1 << 14) | (1 << 15);

  /* 4. Wait 150 cycles – this provides the required hold time for the control signal */
  for(int delay_count = 0; delay_count < 150; ++delay_count)
  {
    /* Same as above */
  }

  /* 5. Write to GPPUD to remove the control signal */
  /* We don't need to modify it here */

  /* 6. Write to GPPUDCLK0/1 to remove the clock */
  *GPPUDCLK0 = 0;

  /* Clear interrupt */
  *UART_ICR = 0x7FF;

  /* Setup uart bauldrate
   *
   * BAUDDIV = (FUARTCLK/(16 * Baud rate))
   * BAUDDIV = IBRD + ROUND_UP(FBRD / 64)
   * 64 because BAUDDIV is 6 bit and 2 ** 6 = 64
   * FUARTCLK = uart clock rate
   *
   * BAUDDIV = 3000000 / (16 * 115200) = 1.62670416666
   * IBRD = 1
   * FBRD = 0.6267 * 64 = 40.10 -> round up to 41
   */
  *UART_IBRD = 1;
  *UART_FBRD = 41;

  /* setup line control to 8 databits
   *
   * WLEN[6:5]
   * b11 = 8 bits
   * b10 = 7 bits
   * b01 = 6 bits
   * b00 = 5 bits.
   *
   * FEN[4] DONT USE NOT WORK AS INTEDED
   * 1 enable fifo
   *
   */
  *UART_LCRH = 3 << 5;
  //*UART_LCRH = 0x70;


  /* reenable cr
   * set bit rxe [9], txe[8], uarten[0]
   * which is 0x301
   */
  *UART_CR = 0x301;

  uart_puts_blocking("UART init complete\n");
  return;
}

/* put a character to uart, blocking io */
char uart_putc(const char c)
{
  while(!CHECK_BIT(*UART_FR, 7)); /* If txfe [7] bit is set, fifo is empty */
  *UART_DR = (uint32_t)c;
  return c;
}

/* Write string to uart, blocking */
void uart_puts(const char * string)
{
  uart_puts_blocking(string);
  return;
}

void uart_puts_blocking(const char * string)
{
  /* BACKUP: Incase interrupt not working */
  for(unsigned idx = 0; string[idx] != '\0'; ++idx)
  {
    while(!CHECK_BIT(*UART_FR, 7)); /* If txfe [7] bit is set, fifo is empty */
    *UART_DR = (uint32_t)string[idx];
  }
}

