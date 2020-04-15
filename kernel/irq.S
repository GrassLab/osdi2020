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
