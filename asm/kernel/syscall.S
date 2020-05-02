/*
   syscall_x8 (x0, x1, x2, x3)
*/
.section ".text"
.global system_call
system_call:
  adr x9, syscall_tables
  lsl x8, x8, #3
  add x8, x8, x9
  adr x10, end_syscall_tables
  cmp x8, x10
  b.hs fail
  cmp x8, x9
  b.lo fail
  ldr x8, [x8]
  br x8
fail:
  bl syscall_number_error
