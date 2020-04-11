// load exception_table to VBAR_EL2
ldr x0, =exception_table
msr VBAR_EL2, x0

// Simple vector table
.align 11 // vector table should be aligned to 0x800
.global exception_table
exception_table:
    b exception_handler // branch to a handler function.
    .align 7 // entry size is 0x80, .align will pad 0
    b exception_handler
    .align 7
    b exception_handler
    .align 7
    b exception_handler
    .align 7

    b sync_currentEL_SP_ELx
    .align 7
    b exception_handler
    .align 7
    b exception_handler
    .align 7
    b exception_handler
    .align 7

    b exception_handler
    .align 7
    b exception_handler
    .align 7
    b exception_handler
    .align 7
    b exception_handler
    .align 7

    b exception_handler
    .align 7
    b exception_handler
    .align 7
    b exception_handler
    .align 7
    b exception_handler
    .align 7

.global exception_handler
exception_handler:
    b exception_handler

sync_currentEL_SP_ELx;
    mrs x0, ELR_EL2
    mrs x1, ESR_EL2
    bl el2_sync_message