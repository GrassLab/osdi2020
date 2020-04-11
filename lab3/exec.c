#include "exec.h"

void exec_dispatcher(uint64_t identifier)
{
  /* identifier is 0xYZ */
  /* Y is exception level */
  /* Z is offset of Z * 0x80 */
  uint8_t level = (uint8_t)(identifier & 0x20) >> 4;
  uint8_t type = (uint8_t)(identifier & 0xf);
  switch(level)
  {
  case 3:
    break;
  case 2:
    switch(type)
    {
    case 4:
      exec_EL2_current_EL_SP_EL2_sync();
      return;
    default:
      exec_not_implemented();
      return;
    }
    break;
  case 1:
    break;
  case 0: /* EL 0 */
  default:
    break;
  }
  return;
}

void exec_not_implemented(void)
{
  uart_puts("Not Implement\n");
  return;
}

void exec_EL2_current_EL_SP_EL2_sync(void)
{
  uart_puts("Hi");
  return;
}
