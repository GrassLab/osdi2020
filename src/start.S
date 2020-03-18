.section ".text.boot"

.global _start

_start:
  mrs x0, mpidr_el1
  and x0, x0, #3
  cbz x0, 1f
0:
  wfe
  b 0b

1:
  ldr x0, =_start
  mov sp, x0
  ldr x0, __bss_start
  ldr x1, __bss_end
  sub x1, x1, x0
  lsr x1, x1, #3
2:
  cbz x1, 3f
  str xzr, [x0]
  add x0, x0, #8
  sub x1, x1, #1
  b 2b
3:
  bl main
