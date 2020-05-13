#include "libc.h"
#include "signal.h"

/* TODO */
sig_t signal(int sig, sig_t func) {
  /* set the signal handler(func) for the specified signal type(sig) */
  /* and return the previos signal handler */

  /**
   * task_struct
   *  +----------+
   *  |   ...    |
   *  | signals -+---> [signal info] -> ... -> [signal info]
   *  | sighand -+---> [sigact, sigact, ..., sigact ]
   *  +----------+
   */

  return 0;
}
/* pid 3: kill */
/* pid , SIGKILL  */
void send_signal(int pid, int code) {
  task[pid]->signals |= code;
}
