#include <syscall.h>
#include "time.h"

double
get_time_double ()
{
  size_t cnt, freq;
  get_time (&cnt, &freq);
  return (double) cnt / (double) freq;
}

void
delay (double sec)
{
  double t;
  t = get_time_double ();
  while (get_time_double () - t < sec);
}
