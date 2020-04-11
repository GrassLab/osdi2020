.global EL2_vector

// Ref: https://developer.arm.com/docs/100933/0100/example-exception-handlers
// Note: x30 is link register, crucial to preserve
// Note: The macro is more than 0x80 bytes, you should branch it first
.macro ENTER_DISPATCHER
  stp x0, x1,   [sp, #-16]!
  stp x29, x30, [sp, #-16]!
  eor x0, x0, x0
.endm

.macro LEAVE_DISPATCHER
  ldp x29, x30, [sp], #16
  ldp x0, x1,   [sp], #16
  eret
.endm

.text
EL_dispatcher:
  stp x2, x3,   [sp, #-16]!
  stp x4, x5,   [sp, #-16]!
  stp x6, x7,   [sp, #-16]!
  stp x8, x9,   [sp, #-16]!
  stp x10, x11, [sp, #-16]!
  stp x12, x13, [sp, #-16]!
  stp x14, x15, [sp, #-16]!
  stp x16, x17, [sp, #-16]!
  stp x18, x19, [sp, #-16]!
  stp x20, x21, [sp, #-16]!
  stp x22, x23, [sp, #-16]!
  stp x24, x25, [sp, #-16]!
  stp x26, x27, [sp, #-16]!
  stp x28, x29, [sp, #-16]!
  str x30,      [sp, #-8]!

  bl exc_dispatcher

  ldr x30,      [sp], #8
  ldp x28, x29, [sp], #16
  ldp x26, x27, [sp], #16
  ldp x24, x25, [sp], #16
  ldp x22, x23, [sp], #16
  ldp x20, x21, [sp], #16
  ldp x18, x19, [sp], #16
  ldp x16, x17, [sp], #16
  ldp x14, x15, [sp], #16
  ldp x12, x13, [sp], #16
  ldp x10, x11, [sp], #16
  ldp x8, x9,   [sp], #16
  ldp x6, x7,   [sp], #16
  ldp x4, x5,   [sp], #16
  ldp x2, x3,   [sp], #16
  ret

.align 11 // aligned to 0x800
EL2_vector:
// from current EL using EL0
  ENTER_DISPATCHER
  add x0, x0, #0x20
  bl EL_dispatcher
  LEAVE_DISPATCHER
.align 7
  ENTER_DISPATCHER
  add x0, x0, #0x21
  bl EL_dispatcher
  LEAVE_DISPATCHER
.align 7
  ENTER_DISPATCHER
  add x0, x0, #0x22
  bl EL_dispatcher
  LEAVE_DISPATCHER
.align 7
  ENTER_DISPATCHER
  add x0, x0, #0x23
  bl EL_dispatcher
  LEAVE_DISPATCHER
.align 7

// from current EL using ELX
  ENTER_DISPATCHER
  add x0, x0, #0x24
  bl EL_dispatcher
  LEAVE_DISPATCHER
.align 7

// The rest are not implemented yet


