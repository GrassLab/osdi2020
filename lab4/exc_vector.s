// Ref: https://developer.arm.com/docs/100933/0100/example-exception-handlers
// Note: x30 is link register, crucial to preserve
// Note: The macro is more than 0x80 bytes, you should branch it first
.macro ENTER_DISPATCHER
  str x0,       [sp, #-16]!
  stp x29, x30, [sp, #-16]!

  eor x0, x0, x0
.endm

.macro LEAVE_DISPATCHER
  ldp x29, x30, [sp], #16
  ldr x0,       [sp], #16
  eret
.endm

.text
EL_dispatcher:
  sub	sp, sp, #272

  stp	x1, x2,   [sp, #16 * 0]
  stp	x3, x4,   [sp, #16 * 1]
  stp	x5, x6,   [sp, #16 * 2]
  stp	x7, x8,   [sp, #16 * 3]
  stp	x9, x10,  [sp, #16 * 4]
  stp	x11, x12, [sp, #16 * 5]
  stp	x13, x14, [sp, #16 * 6]
  stp	x15, x16, [sp, #16 * 7]
  stp	x17, x18, [sp, #16 * 8]
  stp	x19, x20, [sp, #16 * 9]
  stp	x21, x22, [sp, #16 * 10]
  stp	x23, x24, [sp, #16 * 11]
  stp	x25, x26, [sp, #16 * 12]
  stp	x27, x28, [sp, #16 * 13]
  stp	x29, x30, [sp, #16 * 14]

  mrs x1, spsr_el1
  mrs x2, elr_el1
  mrs x3, sp_el0
  stp x1, x2,   [sp, #16 * 15]
  str x3,       [sp, #16 * 16]

  bl exc_dispatcher

  ldr x3,       [sp, #16 * 16]
  ldp x1, x2,   [sp, #16 * 15]
  msr sp_el0, x3
  msr elr_el1, x2
  msr spsr_el1, x1

  ldp	x29, x30, [sp, #16 * 14]
  ldp	x27, x28, [sp, #16 * 13]
  ldp	x25, x26, [sp, #16 * 12]
  ldp	x23, x24, [sp, #16 * 11]
  ldp	x21, x22, [sp, #16 * 10]
  ldp	x19, x20, [sp, #16 * 9]
  ldp	x17, x18, [sp, #16 * 8]
  ldp	x15, x16, [sp, #16 * 7]
  ldp	x13, x14, [sp, #16 * 6]
  ldp	x11, x12, [sp, #16 * 5]
  ldp	x9, x10,  [sp, #16 * 4]
  ldp	x7, x8,   [sp, #16 * 3]
  ldp	x5, x6,   [sp, #16 * 2]
  ldp	x3, x4,   [sp, #16 * 1]
  ldp	x1, x2,   [sp, #16 * 0]

  add	sp, sp, #272

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


