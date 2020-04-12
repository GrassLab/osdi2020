.section ".data.exception"

_exception_ret_addr: .asciz "Exception return address "
_exception_class: .asciz "Exception class (EC) "
_exception_iss: .asciz "Instruction specific syndrom (ISS) "

.section ".text.exception"

.align 11 // aligned to 0x800 (2^11)
.global _exception_table
_exception_table:
    b _exception_handler // branch to a handler function.
    .align 7 // entry size is 0x80 (2^7), .align will pad 0
    b _exception_handler
    .align 7
    b _exception_handler
    .align 7
    b _exception_handler
    .align 7

    b _exception_handler
    .align 7
    b _exception_handler
    .align 7
    b _exception_handler
    .align 7
    b _exception_handler
    .align 7

    b _exception_handler
    .align 7
    b _exception_handler
    .align 7
    b _exception_handler
    .align 7
    b _exception_handler
    .align 7

    b _exception_handler
    .align 7
    b _exception_handler
    .align 7
    b _exception_handler
    .align 7
    b _exception_handler
    .align 7

hang:
    wfe
    b hang

.global _exception_handler
_exception_handler:
    ldr x0, =_exception_ret_addr
    bl sendStringUART
    mrs x0, ELR_EL2
    bl sendHexUART
    mov x0, #10
    bl sendUART

    ldr x0, =_exception_class
    bl sendStringUART
    mrs x0, ESR_EL2
    // logical shift right
    // EC: [31:26]
    lsr x0, x0, #26
    bl sendHexUART
    mov x0, #10
    bl sendUART

    ldr x0, =_exception_iss
    bl sendStringUART
    mrs x0, ESR_EL2
    // ISS: [24:0], 0x1ffffff (2**25 - 1)
    and x0, x0, #0x1ffffff
    bl sendHexUART
    mov x0, #10
    bl sendUART

    b hang
