#include "stackguard.h"

void
__stack_chk_fail (void)
{
  char buf[0x30] = "Are you hacker?";
  while (1);
}