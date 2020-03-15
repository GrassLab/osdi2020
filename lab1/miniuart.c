#include "miniuart.h"
#include "aux.h"

void miniuart_init(void)
{
  /* Setup GPIO */
  /* TODO: Diverge */
  /* TODO */


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


}

