.section ".text.boot"

.global _start

_start:
    // read cpu id, stop slave cores
    mrs     x1, mpidr_el1
    and     x1, x1, #3
    cbz     x1, _id0
    // cpu id > 0, stop
_buzy_loop:
    wfe
    b       _buzy_loop

_id0:
    adr x0, exception_table
    msr vbar_el1, x0
    mov x0, #(1 << 31) // EL1 use aarch64
    msr hcr_el2, x0

    // set stack before our code
    ldr     x0, =_start
    add x0, x0, #0x4000
    mov     sp, x0

    add x0, x0, #0x2000
    msr sp_el0, x0
    add x0, x0, #0x2000
    msr sp_el1, x0

    // clear bss
    ldr     x1, =__bss_start
    ldr     x2, =__bss_size

_clear_bss:
    cbz     x2, from_el2_to_el1 
    str     xzr, [x1], #8
    sub     x2, x2, #1
    cbnz    x2, _clear_bss

from_el2_to_el1:
  mov x0, 0x5//3c5 // EL1h (SPSel = 1) with interrupt disabled
  msr spsr_el2, x0
  adr x0, _shell//from_el1_to_el0   // load exception return address
  msr elr_el2, x0
  eret // return to EL1



#define USER_STACK 0x1000

#from_el1_to_el0:
#  mov x0, USER_STACK
#  msr sp_el0, x0
#  mov x0, 0 // EL0 with interrupt enabled
#  msr spsr_el1, x0
#  adr x0, _shell// return to shell run in EL0
#  msr elr_el1, x0
#  eret


_shell:
    // jump to relocated C code, should not return
    bl      main
    // for failsafe, halt this core too
    b       _buzy_loop
