#include "syscall.h"
.section ".text"
.global uart_read
uart_read:
  mov x8, #2
  svc #0
  ret
