#include <string.h>
#include "panic.h"

void
not_implemented ()
{
  printf ("kernel panic - function not implemented!");
  while (1);
}

void
syscall_number_error ()
{
  printf ("kernel panic - syscall number error!");
  while (1);
}
