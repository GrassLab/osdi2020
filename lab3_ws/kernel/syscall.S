.macro cts_s
    sub	sp, sp, #0xB0
    stp	x10, x11, [sp, #0x10 * 0x0]
    stp	x12, x13, [sp, #0x10 * 0x1]
    stp	x14, x15, [sp, #0x10 * 0x2]
    stp	x16, x17, [sp, #0x10 * 0x3]
    stp	x18, x19, [sp, #0x10 * 0x4]
    stp	x20, x21, [sp, #0x10 * 0x5]
    stp	x22, x23, [sp, #0x10 * 0x6]
    stp	x24, x25, [sp, #0x10 * 0x7]
    stp	x26, x27, [sp, #0x10 * 0x8]
    stp	x28, x29, [sp, #0x10 * 0x9]
    str	x30, [sp, #0x10 * 0xA] 
.endm

.macro  cts_l
    ldp	x10, x11, [sp, #16 * 0]
    ldp	x12, x13, [sp, #16 * 1]
    ldp	x14, x15, [sp, #16 * 2]
    ldp	x16, x17, [sp, #16 * 3]
    ldp	x18, x19, [sp, #16 * 4]
    ldp	x20, x21, [sp, #16 * 5]
    ldp	x22, x23, [sp, #16 * 6]
    ldp	x24, x25, [sp, #16 * 7]
    ldp	x26, x27, [sp, #16 * 8]
    ldp	x28, x29, [sp, #16 * 9]
    ldr	x30, [sp, #16 * 10] 
    add	sp, sp, #0xB0
.endm

.global do_sys
do_sys:
	/* store regs */
    cts_s
    lsl     x0, x0, #2
    ldr     x10, =syscall_table
	add		x0, x0, x10
	blr		x0
    cts_l
    ret

syscall_table:
	b sys_core_timer_init	// 0
	b sys_getfreq			// 1
	b sys_getcurrentcount	// 2
	b sys_local_timer_init	// 3
	b sys_core_timer_init	// 4
	b sys_sys_timer_init	// 5
	b sys_reset				// 6
	b sys_cancel_reset		// 7
    b show_exc				// 8