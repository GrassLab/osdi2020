#include "tlb.h"

void
tlb_init ()
{
  // kernel
  // ffff000000000000-ffff000000200000 -> 000000000000-000000200000
  // peripheral
  // ffffaaaa00000000-ffffaaaa01000000 -> 00003f000000-000040000000
  // arm local
  // ffffaaaa01000000-ffffaaaa01040000 -> 000040000000-000040040000
}
