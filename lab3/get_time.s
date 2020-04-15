.globl get_cntfrq_el0
.globl get_cntpct_el0
.type get_cntfrq_el0,%function
.type get_cntpct_el0,%function

get_cntfrq_el0:
  mrs x0, cntfrq_el0
  ret

get_cntpct_el0:
  mrs x0, cntpct_el0
  ret

