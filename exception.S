  .section ".text"
  .macro kernel_entry
  sub   sp, sp, #16 * 1
  str 	x30, [sp, #16 * 0] 
  sub	sp, sp, #16 * 15
  stp	x0, x1, [sp, #16 * 0]
  stp	x2, x3, [sp, #16 * 1]
  stp	x4, x5, [sp, #16 * 2]
  stp	x6, x7, [sp, #16 * 3]
  stp	x8, x9, [sp, #16 * 4]
  stp	x10, x11, [sp, #16 * 5]
  stp	x12, x13, [sp, #16 * 6]
  stp	x14, x15, [sp, #16 * 7]
  stp	x16, x17, [sp, #16 * 8]
  stp	x18, x19, [sp, #16 * 9]
  stp	x20, x21, [sp, #16 * 10]
  stp	x22, x23, [sp, #16 * 11]
  stp	x24, x25, [sp, #16 * 12]
  stp	x26, x27, [sp, #16 * 13]
  stp	x28, x29, [sp, #16 * 14]
  .endm

  .macro kernel_exit
  ldr	x30, [sp, #16 * 0] 
  ldp	x0, x1, [sp, #16 * 1]
  ldp	x2, x3, [sp, #16 * 2]
  ldp	x4, x5, [sp, #16 * 3]
  ldp	x6, x7, [sp, #16 * 4]
  ldp	x8, x9, [sp, #16 * 5]
  ldp	x10, x11, [sp, #16 * 6]
  ldp	x12, x13, [sp, #16 * 7]
  ldp	x14, x15, [sp, #16 * 8]
  ldp	x16, x17, [sp, #16 * 9]
  ldp	x18, x19, [sp, #16 * 10]
  ldp	x20, x21, [sp, #16 * 11]
  ldp	x22, x23, [sp, #16 * 12]
  ldp	x24, x25, [sp, #16 * 13]
  ldp	x26, x27, [sp, #16 * 14]
  ldp	x28, x29, [sp, #16 * 15]
  add	sp, sp, #16 * 16
  eret
  .endm

_exc_handler:
  kernel_entry
    mov x0, #0
    mrs     x1, esr_el2
    mrs     x2, elr_el2
    mrs     x3, spsr_el2
    mrs     x4, far_el2
  bl exc_handler
  kernel_exit

_synchronous_handler:
/*
  kernel_entry
    mrs     x0, esr_el2
    mrs     x1, elr_el2
    mrs     x2, spsr_el2
    mrs     x3, far_el2
    */
  bl synchronous_handler
  eret
  //kernel_exit


// exception
    // important, code has to be properly aligned
    .align 11
    .global _exception_vector
_exception_vector:
    // synchronous
    .align  7
    b       _exc_handler
    // IRQ
    .align  7
    b       _exc_handler
    // FIQ
    .align  7
    b       _exc_handler
    // SError
    .align  7
    b       _exc_handler


    // synchronous
    .align  7
    b       _synchronous_handler
    // IRQ
    .align  7
    b       _exc_handler
    // FIQ
    .align  7
    b       _exc_handler
    // SError
    .align  7
    b       _exc_handler

    // synchronous
    .align  7
    b       _exc_handler
    // IRQ
    .align  7
    b       _exc_handler
    // FIQ
    .align  7
    b       _exc_handler
    // SError
    .align  7
    b       _exc_handler

    // synchronous
    .align  7
    b       _exc_handler
    // IRQ
    .align  7
    b       _exc_handler
    // FIQ
    .align  7
    b       _exc_handler
    // SError
    .align  7
    b       _exc_handler