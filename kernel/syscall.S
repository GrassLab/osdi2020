.global el1_to_el0
el1_to_el0:
  msr elr_el1, x0
  msr sp_el0, x1
  msr spsr_el1, xzr
  eret

//.global do_fork
//do_fork:
//  mov x0, sp
//  mov x1, lr
//
//  sub sp, sp, #16
//  str lr, [sp]
//  bl do_fork_helper
//  ldr lr, [sp]
//  add sp, sp, #16
//  ret
//
//.global post_fork_child_hook
//post_fork_child_hook:
//  mov x0, #0
//  blr x19
