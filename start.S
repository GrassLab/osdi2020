.section ".text.boot"
_start:
  # get cpuid
  mrs x1, mpidr_el1
  and x1, x1, #0xff
  # jmp if cpuid != 0
  cbnz x1, loop

  # clear bss
  ldr x1, =__bss_start
  ldr x2, =__bss_end
  b cmp_adr
set_zero:
  str xzr, [x1]
  add x1, x1, #8
cmp_adr:
  cmp x2, x1
  b.hi set_zero

check_EL2:
  mrs x0, CurrentEL
  lsr x0, x0, #2
  cmp x0, #2
  # setup exception table before switch EL
  ldr x0, =_exception_vector // in exception.S
  msr vbar_EL1, x0

  # setup EL1 stack
  adr x0, _start
  msr sp_el1, x0

  # set RW (AArch64)
  mov x0, #(1 << 31)
  msr hcr_el2, x0
  mrs x0, hcr_el2

  # switch to EL1
  mov x0, 0x5
  msr spsr_el2, x0
  adr x0, ret2EL1
  msr elr_el2, x0
  eret

ret2EL1:
  # disable exception of access SIMD register in EL1
  mrs x0, CPACR_EL1
  orr x0, x0, #(3 << 20)
  msr CPACR_EL1, x0
  # setup EL0 stack
  adr x0, _start
  sub x0, x0, #0x10000
  msr sp_el0, x0
/*
  # switch to EL0
  # spsr_el1 clear daif and switch to EL0
  mov x0, 0x0
  msr spsr_el1, x0
  adr x0, ret2EL0
  msr elr_el1, x0
  eret


ret2EL0:
  mov x0, xzr
*/


b main

loop:
  wfe
  b loop
