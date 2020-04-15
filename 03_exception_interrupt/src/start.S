.section ".text.boot"
.global _start

_start:
    mrs  x1, mpidr_el1
    and  x1, x1, #3
    cbz  x1, _init

.global  _loop
_loop:
    wfe
    b    _loop

_init:
    bl   init_for_el1
    bl   from_el2_to_el1

_init2:
    ldr  x1, =_start
    mov  sp, x1
    
    ldr  x1, =__bss_start
    ldr  w2, =__bss_size

_zero:
    cbz  w2, _jmain
    str  xzr, [x1], #8
    sub  w2, w2, #1
    cbnz w2, _zero

_jmain:
    // bl   init_el2_vector
    // bl   enable_interrupt_el2
    bl from_el1_to_el0
    bl   main
    b    _loop

from_el2_to_el1:
  mov x0, (1 << 31) // EL1 use aarch64
  msr hcr_el2, x0
  mov x0, 0x3c5 // EL1h (SPSel = 1) with interrupt disabled
  msr spsr_el2, x0
  adr x0, _init2 // load exception return address
  msr elr_el2, x0
  adr x0, _start // init sp for el1 option 1
  msr sp_el1, x0
  eret // return to EL1

#define USER_STACK 0x1000
from_el1_to_el0:
  mov x0, USER_STACK
  msr sp_el0, x0
  mov x0, 0 // EL0 with interrupt enabled
  msr spsr_el1, x0
  adr x0, main // return to shell run in EL0
  msr elr_el1, x0
  eret
