#include "miniuart.h"
#include "aux.h"
#include "gpio.h"

void miniuart_init(void)
{
  /* Setup GPIO */
  /* TODO: Diverge */
  /* Configure GPFSELn register to change alternate function */
  /* GPIO 14 [14:12], 15 [17:15] belongs to GPFSEL1 */
  /* miniuart is ALT5 for GPIO 14, 15 */
  /* ALT5 = 010 = 2*/
  *GPFSEL1 &= (uint32_t)~(7 << 12); /* clear GPIO14 [14:12] */
  *GPFSEL1 &= (uint32_t)~(7 << 15); /* clear GPIO15 [17:15] */
  *GPFSEL1 |= (2 << 12); /* set GPIO14 to ALT5 */
  *GPFSEL1 |= (2 << 15); /* set GPIO14 to ALT5 */

  /* disable GPIO pull up/down */
  /* If you use a particular pin as input and don't connect anything to this pin,
   * you will not be able to identify whether the value of the pin is 1 or 0.
   * In fact, the device will report random values. The pull-up/pull-down mechanism
   * allows you to overcome this issue. If you set the pin to the pull-up state and
   * nothing is connected to it, it will report 1 all the time (for the pull-down state,
   * the value will always be 0). The pin state is preserved even after a reboot, so
   * before using any pin, we always have to initialize its state. */

  /* 1. Write to GPPUD to set the required control signal */
  *GPPUD = 0; /* 00 = Off – disable pull-up/down */

  /* 2. Wait 150 cycles – this provides the required set-up time for the control signal */
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


  /* Setup miniuart */

  /* 1. Set AUXENB register to enable mini UART. Then mini UART register can be accessed */
  *AUX_ENABLES |= 1;

  /* 2. Set AUX_MU_CNTL_REG to 0. Disable transmitter and receiver during configuration. */
  *AUX_MU_CNTL_REG = 0;

  /* 3. Set AUX_MU_IER_REG to 0. Disable interrupt because currently you don’t need interrupt. */
  *AUX_MU_IER_REG = 0;

  /* 4. Set AUX_MU_LCR_REG to 3. Set the data size to 8 bit. */
  *AUX_MU_LCR_REG = 3;

  /* 5. Set AUX_MU_MCR_REG to 0. Don’t need auto flow control. */
  *AUX_MU_MCR_REG = 0;

  /* 6. Set AUX_MU_BAUD to 270. Set baud rate to 115200 */
  *AUX_MU_BAUD_REG = 270;

  /* 7. Set AUX_MU_IIR_REG to 6. No FIFO. */
  /* TODO: Diverge */
  *AUX_MU_IIR_REG = 6;

  /* 8. Set AUX_MU_CNTL_REG to 3. Enable the transmitter and receiver. */
  *AUX_MU_CNTL_REG = 3;

  return;
}

/* get a character from miniuart, blocking io */
char miniuart_getc(void)
{
  while(1)
  {
    if(CHECK_BIT(*AUX_MU_LSR_REG, 0)) /* If data ready [0] bit is set */
    {
      return (char)(*AUX_MU_IO_REG & 0xff);
    }
  }
}

/* put a character to miniuart, blocking io */
char miniuart_putc(char c)
{
  while(1)
  {
    if(CHECK_BIT(*AUX_MU_LSR_REG, 5)) /* If transmitter empty [5] bit is set */
    {
      *AUX_MU_IO_REG = (uint32_t)c;
      return c;
    }
  }
}


