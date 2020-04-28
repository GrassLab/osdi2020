.global el1_to_el0
el1_to_el0:
  msr elr_el1, x0
  msr sp_el0, x1
  msr spsr_el1, xzr
  eret
