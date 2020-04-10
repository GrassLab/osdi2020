.section ".text"
_start:
  # get cpuid
  mrs x1, mpidr_el1
  and x1, x1, #0xff
  # jmp if cpuid != 0
  cbnz x1, loop

  # init bss
  ldr x1, =__bss_start
  ldr x2, =__bss_end
  b cmp_adr
set_zero:
  str xzr, [x1]
  add x1, x1, #8
cmp_adr:
  cmp x2, x1
  b.hi set_zero

  # init stack
  mov sp, #0x80000

  # jmp to main
  b main
 
loop:
  wfe
  b loop
