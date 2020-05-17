/*
exception_entry:
	push x0~x30, SP_EL0, ELR_EL1, SPSR_EL1
exception_handler:
	syscall or show exception
exception_exit:
	pop x0~x30, SP_EL0, ELR_EL1, SPSR_EL1
irq_entry:
	push x0~x30, SP_EL0, ELR_EL1, SPSR_EL1
	swith to IRQ stack
irq_hander:
	if (core timer) set reschedule bit
irq_exit:
	swith back to kernel task stack
	schedule
	pop x0~x30, SP_EL0, ELR_EL1, SPSR_EL1
*/
.section ".text"
  .macro kernel_entry
  sub	sp, sp, #16 * 17
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
  mrs 	x9, SP_EL0
  stp	x30, x9, [sp, #16 * 15]
  mrs 	x9, ELR_EL1
  mrs 	x10, SPSR_EL1
  stp 	x9, x10, [sp, #16 * 16]
  .endm

  .macro kernel_exit
  ldp 	x30, x9, [sp, #16 * 15]
  msr 	SP_EL0, x9
  ldp 	x9, x10, [sp, #16 * 16]
  msr 	ELR_EL1, x9
  msr 	SPSR_EL1, x10
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
  add	sp, sp, #16 * 17
  eret
  .endm

.global setup_exception_vector_table
setup_exception_vector_table:
  ldr x0, =exception_table
  msr vbar_EL1, x0
  ret

.global do_fork_child
exception_handler:
  kernel_entry
  mrs x9, ESR_EL1
  mov x10, x9
  lsr x9, x9, #26
  cmp x9, #0x15
  b.ne not_syscall
  and x10, x10, 0xffffff
  cmp x10, 0
  b.ne not_syscall
  bl enable_irq
  bl system_call
  // set return value
  str x0, [sp, #16 * 0]
do_fork_child:
  bl disable_irq
exc_end:
  kernel_exit
not_syscall:
  bl exception_router
  b exc_end

irq_handler:
  kernel_entry

  # setup IRQ stack
  adr x9, _start
  mov x10, sp
  sub sp, x9, 16
  str x10, [sp]

  # interrupt handler
  bl irq_router

  # restore task stack
  ldr x10, [sp]
  mov sp, x10

  bl schedule

  kernel_exit

.align 11
.global exception_table
exception_table:
  b not_implemented
  .align 7
  b not_implemented
  .align 7
  b not_implemented
  .align 7
  b not_implemented
  .align 7

  b not_implemented
  .align 7
  b irq_handler
  .align 7
  b not_implemented
  .align 7
  b not_implemented
  .align 7

  b exception_handler
  .align 7
  b irq_handler
  .align 7
  b not_implemented
  .align 7
  b not_implemented
  .align 7

  b not_implemented
  .align 7
  b not_implemented
  .align 7
  b not_implemented
  .align 7
  b not_implemented
  .align 7
