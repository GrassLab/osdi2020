.macro	kernel_entry
sub	sp, sp, #256
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
stp	x22, x23, [sp, #16 * 11]
stp	x24, x25, [sp, #16 * 12]
stp	x26, x27, [sp, #16 * 13]
stp	x28, x29, [sp, #16 * 14]
str	x30, [sp, #16 * 15] 
stp	x20, x21, [sp, #16 * 10]
.endm

.macro	kernel_exit
ldp	x0, x1, [sp, #16 * 0]
ldp	x2, x3, [sp, #16 * 1]
ldp	x4, x5, [sp, #16 * 2]
ldp	x6, x7, [sp, #16 * 3]
ldp	x8, x9, [sp, #16 * 4]
ldp	x10, x11, [sp, #16 * 5]
ldp	x12, x13, [sp, #16 * 6]
ldp	x14, x15, [sp, #16 * 7]
ldp	x16, x17, [sp, #16 * 8]
ldp	x18, x19, [sp, #16 * 9]

ldp	x22, x23, [sp, #16 * 11]
ldp	x24, x25, [sp, #16 * 12]
ldp	x26, x27, [sp, #16 * 13]
ldp	x28, x29, [sp, #16 * 14]
ldr	x30, [sp, #16 * 15] 
ldp	x20, x21, [sp, #16 * 10]
add	sp, sp, #256		
.endm

exception_entry:
    kernel_entry
    mrs     x0, esr_el1
    mrs     x1, elr_el1
    bl exception_handler
    kernel_exit
    eret

irq_entry:
    kernel_entry
    bl irq_handler
    kernel_exit
    eret

.align 11 // vector table should be aligned to 0x800
.global vector_table
vector_table:
  b dummy_handler // branch to a handler function.
  .align 7 // entry size is 0x80, .align will pad 0
  b dummy_handler
  .align 7
  b dummy_handler
  .align 7
  b dummy_handler
  .align 7

  b exception_entry
  .align 7
  b irq_entry
  .align 7
  b dummy_handler
  .align 7
  b dummy_handler
  .align 7

  b exception_entry
  .align 7
  b irq_entry
  .align 7
  b dummy_handler
  .align 7
  b dummy_handler
  .align 7

  b dummy_handler
  .align 7
  b dummy_handler
  .align 7
  b dummy_handler
  .align 7
  b dummy_handler
  .align 7