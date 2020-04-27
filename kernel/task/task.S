.section ".text"

.global switch_to
.global launch_init
.global get_current_task
.global default_task_start

switch_to:
    mov     x9,     sp                      /* save the context of the current task */
    stp     x19,    x20,    [x0], #16
    stp     x21,    x22,    [x0], #16
    stp     x23,    x24,    [x0], #16
    stp     x25,    x26,    [x0], #16
    stp     x27,    x28,    [x0], #16
    stp     fp,     lr,     [x0], #16
    str     x9,             [x0]

    msr     TPIDR_EL1, x1

    ldp     x19,    x20,    [x1], #16       /* restore the context of the next task */
    ldp     x21,    x22,    [x1], #16
    ldp     x23,    x24,    [x1], #16
    ldp     x25,    x26,    [x1], #16
    ldp     x27,    x28,    [x1], #16
    ldp     fp,     lr,     [x1], #16
    ldr     x9,             [x1]
    mov     sp,     x9
    
    ret

launch_init: 
    ldr     x0,     IDLE                    /* IDLE would be declare in task.c */
    
    msr     TPIDR_EL1, x0
    
    ldp     x19,    x20,    [x0], #16       /* restore the context of the next task */
    ldp     x21,    x22,    [x0], #16
    ldp     x23,    x24,    [x0], #16
    ldp     x25,    x26,    [x0], #16
    ldp     x27,    x28,    [x0], #16
    ldp     fp,     lr,     [x0], #16
    ldr     x9,             [x0]
    mov     sp,     x9

    ret

get_current_task:
    mrs     x0,     TPIDR_EL1
    ret

default_task_start:
    blr     x19  
