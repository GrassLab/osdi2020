#include "syscall.h"
.section ".text"
.global uart_read
uart_read:
  mov x8, UART_READ
  svc #0
  ret
.global uart_write
uart_write:
  mov x8, UART_WRITE
  svc #0
  ret
.global exec
exec:
  mov x8, EXEC
  svc #0
  ret
.global get_task_id
get_task_id:
  mov x8, GET_TASK_ID
  svc #0
  ret
.global fork
fork:
  mov x8, FORK
  svc #0
  ret
.global exit
exit:
  mov x8, EXIT
  svc #0
  ret
.global kill
kill:
  mov x8, KILL
  svc #0
  ret
.global page_status
page_status:
  mov x8, PAGE_STATUS
  svc #0
  ret
