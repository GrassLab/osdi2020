.section ".text"

.global switch_to //no store rip
switch_to:

  stp x19, x20, [x0, 16 * 0]
  stp x21, x22, [x0, 16 * 1]
  stp x23, x24, [x0, 16 * 2]
  stp x25, x26, [x0, 16 * 3]
  stp x27, x28, [x0, 16 * 4]
  stp fp, lr, [x0, 16 * 5]
  mov x9, sp
  str x9, [x0, 16 * 6]

  ldp x19, x20, [x1, 16 * 0]
  ldp x21, x22, [x1, 16 * 1]
  ldp x23, x24, [x1, 16 * 2]
  ldp x25, x26, [x1, 16 * 3]
  ldp x27, x28, [x1, 16 * 4]
  ldp fp, lr, [x1, 16 * 5]
  ldr x9, [x1, 16 * 6]
  mov sp,  x9
  msr tpidr_el1, x1
  ret


.global fork_ret
fork_ret:
  stp x19, x20, [x0, 16 * 0]
  stp x21, x22, [x0, 16 * 1]
  stp x23, x24, [x0, 16 * 2]
  stp x25, x26, [x0, 16 * 3]
  stp x27, x28, [x0, 16 * 4]
  sub x9, x1, x29  //x9 = offset of x29 and kernel stack base
  sub x9, x2, x9  // set new kernel stack x29
  stp x9, lr, [x0, 16 * 5]

  mov x9, sp //copy kernel stack
  sub x9, x1, x9
  ldr x10, [x0, 16 * 6]
  sub x10, x10, x9
  str x10, [x0, 16*6]
  mov x9, x9, lsr #3
  mov x13, x10
  mov x11, sp
3:
  cbz x9, 2f
  ldr x12, [x11], #8
  str x12, [x13], #8
  sub x9, x9, #1
  cbnz x9, 3b
2:
  ret
