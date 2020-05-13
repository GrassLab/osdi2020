#include "bh.h"
#include "timer.h"
#include "libc.h"

void bottom_half_enable() {
  sys_core_timer_enable();
}

bh_handler extract_bh_handler() {
  for (int i = 0; i < 32; ++i) {
    if (bh_active[i]) {
      bh_active[i] = 0;
      return bh_base[i];
    }
  }
  return 0;
}



void push_bh_handle(bh_handler job, int priority) {
  uart_println("\e[0;32m[?] Push a bh handler at priority %d\e[0m", priority);
  bh_base[priority] = job;
  bh_active[priority] = 1;
}
