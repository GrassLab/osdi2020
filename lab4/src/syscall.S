#include "syscall.h"

.global get_taskid
get_taskid:
  mov x8, SYS_GET_TASKID
  svc 0
  ret