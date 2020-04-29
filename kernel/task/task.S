.section ".text"

.global switch_to
.global launch_init
.global default_task_start

switch_to:
    # these are all have been save in the current stack
    # mov     x9,     sp                          /* save the context of the current task */
    # stp     x19,    x20,    [x0],   #16
    # stp     x21,    x22,    [x0],   #16
    # stp     x23,    x24,    [x0],   #16
    # stp     x25,    x26,    [x0],   #16
    # stp     x27,    x28,    [x0],   #16
    # stp     fp,     lr,     [x0],   #16
    # str     x9,             [x0]

    msr     TPIDR_EL0, x1

    ldp     x19,    x20,    [x1],   #16         /* restore the context of the next task */
    ldp     x21,    x22,    [x1],   #16
    ldp     x23,    x24,    [x1],   #16
    ldp     x25,    x26,    [x1],   #16
    ldp     x27,    x28,    [x1],   #16
    ldp     fp,     lr,     [x1],   #16
    ldr     x9,             [x1]
    mov     sp,     x9
    
    ret

# from el1 to el0
launch_init: 
    ldr     x0,     IDLE            // IDLE would be declare in task.c
    
    msr     TPIDR_EL0, x0

    ldr     x19,    [x0, 8 * 19]
    ldr     lr,     [x0, 8 * 30]
    
    msr     ELR_EL1, lr             // when turn form el1 to el0, start from here

    ldr     x9,     [x0, 8 * 31]    //  user sp 
    msr     SP_EL0, x9

    mov     sp,     #0x70000        // move the el1 sp to the default

    eret

default_task_start:
    blr     x19  
