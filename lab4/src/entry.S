#define S_FRAME_SIZE 272

.macro	kernel_entry, el
sub	sp, sp, #S_FRAME_SIZE
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

.if	\el == 0
mrs	x21, sp_el0
.endif 

mrs	x22, elr_el1
mrs	x23, spsr_el1
stp	x30, x21, [sp, #16 * 15] 
stp	x22, x23, [sp, #16 * 16]
.endm

.macro	kernel_exit, el
ldp	x22, x23, [sp, #16 * 16]
ldp	x30, x21, [sp, #16 * 15] 
.if	\el == 0
msr	sp_el0, x21
.endif
msr	elr_el1, x22			
msr	spsr_el1, x23
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
ldp	x20, x21, [sp, #16 * 10]
ldp	x22, x23, [sp, #16 * 11]
ldp	x24, x25, [sp, #16 * 12]
ldp	x26, x27, [sp, #16 * 13]
ldp	x28, x29, [sp, #16 * 14]
add	sp, sp, #S_FRAME_SIZE		
.endm

exception_entry:
    kernel_entry 0
    mrs     x0, esr_el1
    mrs     x1, elr_el1
    bl exception_handler
    kernel_exit 0
    eret

el1_irq:
    kernel_entry 1
    bl irq_handler
    kernel_exit 1
    eret

el0_irq:
    kernel_entry 0
    bl irq_handler
    kernel_exit 0
    eret

.global ret_from_child
ret_from_child:
    kernel_exit 0
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
  b el1_irq
  .align 7
  b dummy_handler
  .align 7
  b dummy_handler
  .align 7

  b exception_entry
  .align 7
  b el0_irq
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