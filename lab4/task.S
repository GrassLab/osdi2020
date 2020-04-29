.global switch_to
switch_to:
	stp 	x19, x20, [x0, 16 * 0]
	stp 	x21, x22, [x0, 16 * 1]
	stp 	x23, x24, [x0, 16 * 2]
	stp 	x25, x26, [x0, 16 * 3]
	stp 	x27, x28, [x0, 16 * 4]
	stp 	fp, lr, [x0, 16 * 5]
	mov 	x9, sp
	str 	x9, [x0, 16 * 6]

	ldp 	x19, x20, [x1, 16 * 0]
	ldp 	x21, x22, [x1, 16 * 1]
	ldp 	x23, x24, [x1, 16 * 2]
	ldp 	x25, x26, [x1, 16 * 3]
	ldp 	x27, x28, [x1, 16 * 4]
	ldp 	fp, lr, [x1, 16 * 5]
	ldr 	x9, [x1, 16 * 6]
	mov 	sp,  x9
	ret

.global get_cur_task
get_cur_task:
	mrs 	x0, tpidr_el1
	ret

.global set_cur_task
set_cur_task:
	msr 	tpidr_el1, x0
	ret

.global go_to
go_to:
	ldp 	fp, lr, [x0, 8 * 10]
	ldr 	x9, [x0, 8 * 12]
	mov 	sp,  x9
	ret

.global store_umode_lr_sp
store_umode_lr_sp:
	mrs 	x9, elr_el1
	mrs 	x10, sp_el0
	str 	x9, [x0]
	str 	x10, [x1]
	ret

.global restore_umode_lr_sp
restore_umode_lr_sp:
	msr 	elr_el1, x0
	msr 	sp_el0, x1
	ret

.global restore_all_reg
restore_all_reg:
	ldp x0, x1, [sp ,16 * 0]
	ldp x2, x3, [sp ,16 * 1]
	ldp x4, x5, [sp ,16 * 2]
	ldp x6, x7, [sp ,16 * 3]
	ldp x8, x9, [sp ,16 * 4]
	ldp x10, x11, [sp ,16 * 5]
	ldp x12, x13, [sp ,16 * 6]
	ldp x14, x15, [sp ,16 * 7]
	ldp x16, x17, [sp ,16 * 8]
	ldp x18, x19, [sp ,16 * 9]
	ldp x20, x21, [sp ,16 * 10]
	ldp x22, x23, [sp ,16 * 11]
	ldp x24, x25, [sp ,16 * 12]
	ldp x26, x27, [sp ,16 * 13]
	ldp x28, x29, [sp ,16 * 14]
	ldr x30, [sp, 16 * 15]
	add sp, sp, 32 * 8
	eret