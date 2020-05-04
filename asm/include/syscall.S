#include "syscall.h"
.section ".text"
.global uart_read
uart_read:
  mov x8, #2
  svc #0
  ret
.global uart_write
uart_write:
  mov x8, #3
  svc #0
  ret
.global exec
exec:
  mov x8, #4
  svc #0
  ret
.global get_task_id
get_task_id:
  mov x8, #5
  svc #0
  ret
.global fork
fork:
  mov x8, #6
  svc #0
  ret
.global exit
exit:
  mov x8, #7
  svc #0
  ret
