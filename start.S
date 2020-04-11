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
  ldr x1, =__bss_start
  ldr x2, =__bss_end
  b cmp_adr
set_zero:
  str xzr, [x1]
  add x1, x1, #8
cmp_adr:
  cmp x2, x1
  b.hi set_zero


    // set stack before our code
    ldr     x1, =_start

    // set up EL1
    mrs     x0, CurrentEL
    and     x0, x0, #12 // clear reserved bits

    // running at EL3?
    cmp     x0, #12
    bne     5f
    // should never be executed, just for completeness
    mov     x2, #0x5b1
    msr     scr_el3, x2
    mov     x2, #0x3c9
    msr     spsr_el3, x2
    adr     x2, 5f
    msr     elr_el3, x2
    eret

    // running at EL2?
5:  cmp     x0, #4
    beq     5f
    

    ldr x0, =_exception_vector // in exception.S
    msr VBAR_EL2, x0
    /*
    msr     sp_el1, x1
    // enable CNTP for EL1
    mrs     x0, cnthctl_el2
    orr     x0, x0, #3
    msr     cnthctl_el2, x0
    msr     cntvoff_el2, xzr
    // enable AArch64 in EL1
    mov     x0, #(1 << 31)      // AArch64
    orr     x0, x0, #(1 << 1)   // SWIO hardwired on Pi3
    msr     hcr_el2, x0
    mrs     x0, hcr_el2
    // Setup SCTLR access
    mov     x2, #0x0800
    movk    x2, #0x30d0, lsl #16
    msr     sctlr_el1, x2
    // set up exception handlers
    ldr     x2, =_exception_vector // in exception.S
    msr     vbar_el1, x2
    // change execution level to EL1
    mov     x2, #0x3c4
    msr     spsr_el2, x2
    adr     x2, 5f
    msr     elr_el2, x2
    eret
    */

5:  mov     sp, x1


jmp_main:
  ldr x1, =error_msg_table
  b main

setup_msg_table:
  mov x0, #1
  b jmp_main

loop:
  wfe
  b loop


