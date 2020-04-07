.section ".data"
EEL2: .string "not in EL2\n"
error_msg_table:
.quad EEL2

.section ".text"
_start:
  # get cpuid
  mrs x1, mpidr_el1
  and x1, x1, #0xff
  # jmp if cpuid != 0
  cbnz x1, loop

  # init bss
  ldr x1, =_bstart
  ldr x2, =_bend
  b cmp_adr
set_zero:
  str xzr, [x1]
  add x1, x1, #8
cmp_adr:
  cmp x2, x1
  b.hi set_zero

  # init stack
  mov sp, #0x80000

check_EL2:
  mrs x0, CurrentEL
  lsr x0, x0, #2
  cmp x0, #2
  mov x0, xzr
  b.eq jmp_main

setup_msg_table:
  mov x0, #1
  ldr x1, =error_msg_table

jmp_main:
  b main
 
loop:
  wfe
  b loop
