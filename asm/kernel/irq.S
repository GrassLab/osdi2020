.section ".data"
prev_daif:
.qword 0
.section ".text"
.globl enable_irq
enable_irq:
  msr daifclr, #0xf
  ret

.globl disable_irq
disable_irq:
  msr daifset, #0xf
  ret

.globl init_irq
init_irq:
  mrs x0, hcr_el2
  orr x0, x0, #16
  msr hcr_el2, x0
  ret

.globl critical_entry
critical_entry:
  mrs x0, daif
  adr x1, prev_daif
  str x0, [x1]
  msr daifset, #0xf
  ret

.globl critical_exit
critical_exit:
  adr x1, prev_daif
  ldr x0, [x1]
  msr daif, x0
  ret
