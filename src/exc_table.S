// load exception_table to VBAR_EL2
.section ".text"

.macro save_all
  sub   sp, sp, #256
  stp   x0, x1, [sp, #16 * 0]
  stp   x2, x3, [sp, #16 * 1]
  stp   x4, x5, [sp, #16 * 2]
  stp   x6, x7, [sp, #16 * 3]
  stp   x8, x9, [sp, #16 * 4]
  stp   x10, x11, [sp, #16 * 5]
  stp   x12, x13, [sp, #16 * 6]
  stp   x14, x15, [sp, #16 * 7]
  stp   x16, x17, [sp, #16 * 8]
  stp   x18, x19, [sp, #16 * 9]
  stp   x20, x21, [sp, #16 * 10] 
  stp   x22, x23, [sp, #16 * 11] 
  stp   x24, x25, [sp, #16 * 12] 
  stp   x26, x27, [sp, #16 * 13] 
  stp   x28, x29, [sp, #16 * 14] 
  str   x30, [sp, #16 * 15] 
  .endm
        
  .macro load_all
  ldp   x0, x1, [sp, #16 * 0]
  ldp   x2, x3, [sp, #16 * 1]
  ldp   x4, x5, [sp, #16 * 2]
  ldp   x6, x7, [sp, #16 * 3]
  ldp   x8, x9, [sp, #16 * 4]
  ldp   x10, x11, [sp, #16 * 5]
  ldp   x12, x13, [sp, #16 * 6]
  ldp   x14, x15, [sp, #16 * 7]
  ldp   x16, x17, [sp, #16 * 8]
  ldp   x18, x19, [sp, #16 * 9]
  ldp   x20, x21, [sp, #16 * 10] 
  ldp   x22, x23, [sp, #16 * 11] 
  ldp   x24, x25, [sp, #16 * 12] 
  ldp   x26, x27, [sp, #16 * 13] 
  ldp   x28, x29, [sp, #16 * 14] 
  ldr   x30, [sp, #16 * 15] 
  add   sp, sp, #256    
  eret
  .endm

.macro ventry handler
  b \handler
  .align 7 // entry size is 0x80, .align will pad 0
.endm


// Simple vector table
.align 11 // vector table should be aligned to 0x800
.global exception_table
exception_table:
    b el2_exc				// Synchronous EL1t
    .align  7
	b el2_irq				// IRQ EL1t
    .align  7
	b el2_irq				// FIQ EL1t
    .align  7
	b el2_irq				// Error EL1t
    .align  7

	b el2_exc				// Synchronous EL1h
    .align  7
	b el2_irq 				// IRQ EL1h
    .align  7
	b el2_irq 				// FIQ EL1h
    .align  7
	b el2_irq				// Error EL1h
    .align  7

	b el2_exc				// Synchronous 64-bit EL0
    .align  7
	b el2_irq 				// IRQ 64-bit EL0
    .align  7
	b el2_irq 				// FIQ 64-bit EL0
    .align  7
	b el2_irq				// Error 64-bit EL0
    .align  7

	b el2_exc				// Synchronous 32-bit EL0
    .align  7
	b el2_irq				// IRQ 32-bit EL0
    .align  7
	b el2_irq				// FIQ 32-bit EL0
    .align  7
	b el2_irq				// Error 32-bit EL0
    .align  7


.global el2_exc 
el2_exc:
    save_all
    bl exc_function
    load_all
     
.global el2_irq
el2_irq:
    save_all
    bl local_timer_handler 
    load_all

.global undefined_handler
undefined_handler:
    save_all
    bl undefined_function
    load_all
