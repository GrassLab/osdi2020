.global schedule_switch_context
.type schedule_switch_context,%function

schedule_switch_context:
  /* TODO: remove unwanted register */
  mov x9, sp
  mrs x10, spsr_el1
  mrs x11, elr_el1
  mrs x12, sp_el0

  stp x19, x20, [x0, 16 * 0]
  stp x21, x22, [x0, 16 * 1]
  stp x23, x24, [x0, 16 * 2]
  stp x25, x26, [x0, 16 * 3]
  stp x27, x28, [x0, 16 * 4]
  stp fp, lr,   [x0, 16 * 5]
  stp x9, x10,  [x0, 16 * 6]
  stp x11, x12, [x0, 16 * 7]

  ldp x19, x20, [x1, 16 * 0]
  ldp x21, x22, [x1, 16 * 1]
  ldp x23, x24, [x1, 16 * 2]
  ldp x25, x26, [x1, 16 * 3]
  ldp x27, x28, [x1, 16 * 4]
  ldp fp, lr,   [x1, 16 * 5]
  ldp x9, x10,  [x1, 16 * 6]
  ldp x11, x12, [x1, 16 * 7]

  mov sp, x9
  msr spsr_el1, x10
  msr elr_el1, x11
  msr sp_el0, x12
  msr tpidr_el1, x2
  msr tpidr_el0, x2
  ret

