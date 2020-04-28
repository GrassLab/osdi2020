#include "sched.h"
#include "syscall.h"

uint32_t do_get_taskid(void) {
  return get_current_task()->id;
}
