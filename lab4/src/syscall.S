#include "syscall.h"

.global get_taskid
get_taskid:
  mov x8, SYS_GET_TASKID
  svc 0
  ret

.global get_input
get_input:
  mov x8, SYS_UART_READ
  svc 0
  ret

.global printf_
printf_:
  mov x8, SYS_UART_WRITE
  svc 0
  ret