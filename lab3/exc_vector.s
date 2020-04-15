// Ref: https://developer.arm.com/docs/100933/0100/example-exception-handlers
// Note: x30 is link register, crucial to preserve
// Note: The macro is more than 0x80 bytes, you should branch it first
.macro ENTER_DISPATCHER
  stp x0, x1,   [sp, #-16]!
  stp x29, x30, [sp, #-16]!
  mrs x0, spsr_el1
  str x0,       [sp, #-16]!
  mrs x0, elr_el1
  str x0,       [sp, #-16]!

  eor x0, x0, x0
.endm

.macro LEAVE_DISPATCHER
  ldr x0,       [sp], #16
  msr elr_el1, x0
  ldr x0,       [sp], #16
  msr spsr_el1, x0
  ldp x29, x30, [sp], #16
  ldp x0, x1,   [sp], #16
  eret
.endm

.text
EL_dispatcher:
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
  stp	x20, x21, [sp, #16 * 10]
  stp	x22, x23, [sp, #16 * 11]
  stp	x24, x25, [sp, #16 * 12]
  stp	x26, x27, [sp, #16 * 13]
  stp	x28, x29, [sp, #16 * 14]
  str	x30, [sp, #16 * 15]

  bl exc_dispatcher

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
  ldr	x30, [sp, #16 * 15]
  add	sp, sp, #256
  ret

.align 11 // aligned to 0x800
.global EL1_vector
EL1_vector:
// from current EL using EL0
  ENTER_DISPATCHER
  add x0, x0, #0x10
  bl EL_dispatcher
  LEAVE_DISPATCHER
.align 7
  ENTER_DISPATCHER
  add x0, x0, #0x11
  bl EL_dispatcher
  LEAVE_DISPATCHER
.align 7
  ENTER_DISPATCHER
  add x0, x0, #0x12
  bl EL_dispatcher
  LEAVE_DISPATCHER
.align 7
  ENTER_DISPATCHER
  add x0, x0, #0x13
  bl EL_dispatcher
  LEAVE_DISPATCHER
.align 7

// from current EL using ELX
  ENTER_DISPATCHER
  add x0, x0, #0x14
  bl EL_dispatcher
  LEAVE_DISPATCHER
.align 7
  ENTER_DISPATCHER
  add x0, x0, #0x15
  bl EL_dispatcher
  LEAVE_DISPATCHER
.align 7
  ENTER_DISPATCHER
  add x0, x0, #0x16
  bl EL_dispatcher
  LEAVE_DISPATCHER
.align 7
  ENTER_DISPATCHER
  add x0, x0, #0x17
  bl EL_dispatcher
  LEAVE_DISPATCHER
.align 7

// from lower EL using in aa64
  ENTER_DISPATCHER
  add x0, x0, #0x18
  bl EL_dispatcher
  LEAVE_DISPATCHER
.align 7
  ENTER_DISPATCHER
  add x0, x0, #0x19
  bl EL_dispatcher
  LEAVE_DISPATCHER
// The rest are not implemented yet


