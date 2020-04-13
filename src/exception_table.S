#include "exception_table.h"

    .macro handle_invalid_entry type
    kernel_entry
    mov x0, #\type
    mrs x1, esr_el2
    mrs x2, elr_el2
    bl  show_exception_status
    b   not_implemented
    .endm

    .macro ventry label
    .align 7  // entry size is 0x80, .align will pad 0
    b   \label
    .endm

    .macro kernel_entry
    sub sp, sp, #256
    stp x0, x1, [sp, #16 * 0]
    stp x2, x3, [sp, #16 * 1]
    stp x4, x5, [sp, #16 * 2]
    stp x6, x7, [sp, #16 * 3]
    stp x8, x9, [sp, #16 * 4]
    stp x10, x11, [sp, #16 * 5]
    stp x12, x13, [sp, #16 * 6]
    stp x14, x15, [sp, #16 * 7]
    stp x16, x17, [sp, #16 * 8]
    stp x18, x19, [sp, #16 * 9]
    stp x20, x21, [sp, #16 * 10]
    stp x22, x23, [sp, #16 * 11]
    stp x24, x25, [sp, #16 * 12]
    stp x26, x27, [sp, #16 * 13]
    stp x28, x29, [sp, #16 * 14]
    str x30, [sp, #16 * 15]
    .endm

    .macro kernel_exit
    ldp x0, x1, [sp, #16 * 0]
    ldp x2, x3, [sp, #16 * 1]
    ldp x4, x5, [sp, #16 * 2]
    ldp x6, x7, [sp, #16 * 3]
    ldp x8, x9, [sp, #16 * 4]
    ldp x10, x11, [sp, #16 * 5]
    ldp x12, x13, [sp, #16 * 6]
    ldp x14, x15, [sp, #16 * 7]
    ldp x16, x17, [sp, #16 * 8]
    ldp x18, x19, [sp, #16 * 9]
    ldp x20, x21, [sp, #16 * 10]
    ldp x22, x23, [sp, #16 * 11]
    ldp x24, x25, [sp, #16 * 12]
    ldp x26, x27, [sp, #16 * 13]
    ldp x28, x29, [sp, #16 * 14]
    ldr x30, [sp, #16 * 15]
    add sp, sp, #256
    eret
    .endm

.section ".text"

.align 11 // vector table should be aligned to 0x800
.global exception_table
exception_table:
    ventry sync_invalid_el2t       // Synchronous EL2t
    ventry irq_invalid_el2t        // IRQ EL2t
    ventry fiq_invalid_el2t        // FIQ EL2t
    ventry error_invalid_el2t      // Error EL2t

    ventry sync_el2h_handler       // Synchronous EL2h
    ventry irq_el2h_handler        // IRQ EL2h
    ventry fiq_invalid_el2h        // FIQ EL2h
    ventry error_invalid_el2h      // Error EL2h

    ventry sync_invalid_el0_64     // Synchronous 64-bit EL0
    ventry irq_invalid_el0_64      // IRQ 64-bit EL0
    ventry fiq_invalid_el0_64      // FIQ 64-bit EL0
    ventry error_invalid_el0_64    // Error 64-bit EL0

    ventry sync_invalid_el0_32     // Synchronous 32-bit EL0
    ventry irq_invalid_el0_32      // IRQ 32-bit EL0
    ventry fiq_invalid_el0_32      // FIQ 32-bit EL0
    ventry error_invalid_el0_32    // Error 32-bit EL0

sync_invalid_el2t:
    handle_invalid_entry  SYNC_INVALID_EL2t

irq_invalid_el2t:
    handle_invalid_entry  IRQ_INVALID_EL2t

fiq_invalid_el2t:
    handle_invalid_entry  FIQ_INVALID_EL2t

error_invalid_el2t:
    handle_invalid_entry  ERROR_INVALID_EL2t

fiq_invalid_el2h:
    handle_invalid_entry  FIQ_INVALID_EL2h

error_invalid_el2h:
    handle_invalid_entry  ERROR_INVALID_EL2h

sync_invalid_el0_64:
    handle_invalid_entry  SYNC_INVALID_EL0_64

irq_invalid_el0_64:
    handle_invalid_entry  IRQ_INVALID_EL0_64

fiq_invalid_el0_64:
    handle_invalid_entry  FIQ_INVALID_EL0_64

error_invalid_el0_64:
    handle_invalid_entry  ERROR_INVALID_EL0_64

sync_invalid_el0_32:
    handle_invalid_entry  SYNC_INVALID_EL0_32

irq_invalid_el0_32:
    handle_invalid_entry  IRQ_INVALID_EL0_32

fiq_invalid_el0_32:
    handle_invalid_entry  FIQ_INVALID_EL0_32

error_invalid_el0_32:
    handle_invalid_entry  ERROR_INVALID_EL0_32

sync_el2h_handler:
    kernel_entry
    mrs x0, esr_el2
    mrs x1, elr_el2
    bl  sync_el2h_router
    kernel_exit

irq_el2h_handler:
    kernel_entry
    bl  irq_el2h_router
    kernel_exit
