.section ".text.boot"

.global _start

_start:
    // read cpu id, stop slave cores
    mrs     x1, mpidr_el1
    and     x1, x1, #3
    cbz     x1, 2f
    // cpu id > 0, stop
1:  wfe
    b       1b
2:  // cpu id == 0

    // set stack before our code
    ldr     x1, =_start
    mov     sp, x1

    mrs     x0, CurrentEL
    and     x0, x0, #12 // clear reserved bits

    // clear bss
    ldr     x1, =__bss_start
    ldr     w2, =__bss_size
3:  cbz     w2, 4f
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, 3b
    // load exception_table to VBAR_EL2
    ldr x0, =exception_table
    msr VBAR_EL2, x0

    // jump to C code, should not return
4:  bl      main
    // for failsafe, halt this core too
    b       1b

push_gr:
    str   x0, [sp, #-16]!
    str   x1, [sp, #-16]!
    str   x2, [sp, #-16]!
    str   x3, [sp, #-16]!
    str   x4, [sp, #-16]!
    str   x5, [sp, #-16]!
    str   x6, [sp, #-16]!
    str   x7, [sp, #-16]!
    str   x8, [sp, #-16]!
    str   x9, [sp, #-16]!
    str   x10, [sp, #-16]!
    str   x11, [sp, #-16]!
    str   x12, [sp, #-16]!
    str   x13, [sp, #-16]!
    str   x14, [sp, #-16]!
    str   x15, [sp, #-16]!
    str   x16, [sp, #-16]!
    str   x17, [sp, #-16]!
    str   x18, [sp, #-16]!
    str   x19, [sp, #-16]!
    str   x20, [sp, #-16]!
    str   x21, [sp, #-16]!
    str   x22, [sp, #-16]!
    str   x23, [sp, #-16]!
    str   x24, [sp, #-16]!
    str   x25, [sp, #-16]!
    str   x26, [sp, #-16]!
    str   x27, [sp, #-16]!
    str   x28, [sp, #-16]!
    str   x29, [sp, #-16]!
    ret

pop_gr:
    ldr   x29, [sp], #16  
    ldr   x28, [sp], #16  
    ldr   x27, [sp], #16  
    ldr   x26, [sp], #16  
    ldr   x25, [sp], #16  
    ldr   x24, [sp], #16  
    ldr   x23, [sp], #16  
    ldr   x22, [sp], #16  
    ldr   x21, [sp], #16  
    ldr   x20, [sp], #16  
    ldr   x19, [sp], #16  
    ldr   x18, [sp], #16  
    ldr   x17, [sp], #16  
    ldr   x16, [sp], #16  
    ldr   x15, [sp], #16  
    ldr   x14, [sp], #16  
    ldr   x13, [sp], #16  
    ldr   x12, [sp], #16  
    ldr   x11, [sp], #16  
    ldr   x10, [sp], #16  
    ldr   x9, [sp], #16  
    ldr   x8, [sp], #16  
    ldr   x7, [sp], #16  
    ldr   x6, [sp], #16  
    ldr   x5, [sp], #16  
    ldr   x4, [sp], #16  
    ldr   x3, [sp], #16  
    ldr   x2, [sp], #16  
    ldr   x1, [sp], #16  
    ldr   x0, [sp], #16  
    ret

// Simple vector table
.align 11 // vector table should be aligned to 0x800
.global exception_table
exception_table:
  b exception_handler // branch to a handler function.
  .align 7 // entry size is 0x80, .align will pad 0
  b exception_handler
  .align 7
  b exception_handler
  .align 7
  b exception_handler
  .align 7

test:
  bl push_gr
  mov     x0, #0
  mrs     x1, esr_el2
  mrs     x2, elr_el2
  mrs     x3, spsr_el2
  mrs     x4, far_el2
  bl exception_handler
  bl pop_gr
  eret
  .align 7
  mov     x0, #1
  b exception_handler
  .align 7
  mov     x0, #2
  b exception_handler
  .align 7
  mov     x0, #3
  b exception_handler
  .align 7

  b exception_handler
  .align 7
  b exception_handler
  .align 7
  b exception_handler
  .align 7
  b exception_handler
  .align 7

  b exception_handler
  .align 7
  b exception_handler
  .align 7
  b exception_handler
  .align 7
  b exception_handler
  .align 7
