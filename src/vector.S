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

  b exception_handler
  .align 7
  bl push_gr
  bl core_timer_handler
  bl pop_gr
  eret
  .align 7
  bl exception_handler
  .align 7
  bl exception_handler
  .align 7

  bl push_gr
  mov     x0, #0
  mrs     x1, esr_el1
  mrs     x2, elr_el1
  bl exception_handler
  bl pop_gr
  eret
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

