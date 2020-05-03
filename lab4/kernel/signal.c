#include "libc.h"


typedef void (*sig_t)(int);

sig_t signal(int sig, sig_t func) {
  /* set the signal handler(func) for the specified signal type(sig) */
  /* and return the previos signal handler */

  /**
   * task_struct
   *  +----------+
   *  |   ...    |
   *  | signals -+---> [signal flag] -> ... -> [signal flag]
   *  | sighand -+---> [sigact, sigact, ..., sigact ]
   *  +----------+
   */


}
