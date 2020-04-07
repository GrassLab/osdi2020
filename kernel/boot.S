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
  b.ne setup_msg_table

setup_exception_vector_table:
  ldr x0, =exception_table
  msr vbar_EL2, x0

  mov x0, xzr
jmp_main:
  ldr x1, =error_msg_table
  b main

setup_msg_table:
  mov x0, #1
  b jmp_main
 
loop:
  wfe
  b loop
