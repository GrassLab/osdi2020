.section ".text"

.global launch_init

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

    # enable el1 irq
    bl      irq_el1_enable          // defined in irq.S
    bl      core_timer_enable       // defined in timer.c

    eret