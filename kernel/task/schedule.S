.section ".text"

.global get_current_task_el0
.global set_next_task_el0

get_current_task_el0:
    mrs     x0,     TPIDR_EL0
    ret

set_next_task_el0:
    msr    TPIDR_EL0, x0
    ret
