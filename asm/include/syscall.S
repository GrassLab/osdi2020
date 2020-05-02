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
