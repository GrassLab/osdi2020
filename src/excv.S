.section ".text"

.macro store_all
    sub     sp, sp, #256
   	stp    	x0, x1, [sp, #16 * 0]
   	stp    	x2, x3, [sp, #16 * 1]
   	stp    	x4, x5, [sp, #16 * 2]
   	stp    	x6, x7, [sp, #16 * 3]
   	stp    	x8, x9, [sp, #16 * 4]
   	stp    	x10, x11, [sp, #16 * 5]
   	stp    	x12, x13, [sp, #16 * 6]
   	stp    	x14, x15, [sp, #16 * 7]
   	stp    	x16, x17, [sp, #16 * 8]
   	stp    	x18, x19, [sp, #16 * 9]
   	stp    	x20, x21, [sp, #16 * 10]
   	stp    	x22, x23, [sp, #16 * 11]
   	stp    	x24, x25, [sp, #16 * 12]
   	stp    	x26, x27, [sp, #16 * 13]
   	stp    	x28, x29, [sp, #16 * 14]
   	str    	x30, [sp, #16 * 15]
.endm

.macro load_all
   	ldp    	x0, x1, [sp, #16 * 0]
   	ldp    	x2, x3, [sp, #16 * 1]
   	ldp    	x4, x5, [sp, #16 * 2]
   	ldp    	x6, x7, [sp, #16 * 3]
   	ldp    	x8, x9, [sp, #16 * 4]
   	ldp    	x10, x11, [sp, #16 * 5]
   	ldp    	x12, x13, [sp, #16 * 6]
   	ldp    	x14, x15, [sp, #16 * 7]
   	ldp    	x16, x17, [sp, #16 * 8]
   	ldp    	x18, x19, [sp, #16 * 9]
   	ldp    	x20, x21, [sp, #16 * 10]
   	ldp    	x22, x23, [sp, #16 * 11]
   	ldp    	x24, x25, [sp, #16 * 12]
   	ldp    	x26, x27, [sp, #16 * 13]
   	ldp    	x28, x29, [sp, #16 * 14]
   	ldr    	x30, [sp, #16 * 15]
	add     sp, sp, #256
.endm

// interrupt vector entry
.macro ventry label
    .align 7
	b       \label
.endm


// Simple vector table
.align 11 // vector table should be aligned to 0x800
.global vector_table
vector_table:
    ventry  curr_el_sp0_sync        // 0x000
    ventry  curr_el_sp0_irq         // 0x080
    ventry  curr_el_sp0_fiq         // 0x100
    ventry  curr_el_sp0_serror      // 0x180

    ventry  curr_el_spx_sync        // 0x200
    ventry  curr_el_spx_irq         // 0x280
    ventry  curr_el_spx_fiq         // 0x300
    ventry  curr_el_spx_serror      // 0x380

    ventry  lower_el_aarch64_sync   // 0x400
    ventry  lower_el_aarch64_irq    // 0x480
    ventry  lower_el_aarch64_fiq    // 0x500
    ventry  lower_el_aarch64_serror // 0x580

    ventry  lower_el_aarch64_sync   // 0x600
    ventry  lower_el_aarch64_irq    // 0x680
    ventry  lower_el_aarch64_fiq    // 0x700
    ventry  lower_el_aarch64_serror // 0x780

/**
 * Exception from the current EL while using SP_EL0
 */
curr_el_sp0_sync:
    eret
                         
curr_el_sp0_irq:
    eret
                         
curr_el_sp0_fiq:
    eret
                         
curr_el_sp0_serror:
    eret

/**
 * Exception from the curren EL while using SP_ELx
 */
curr_el_spx_sync:
    eret                     
curr_el_spx_irq:
    eret
curr_el_spx_fiq:
    eret

curr_el_spx_serror:
    eret


/**
 * Exception from a lower EL and at least one lower EL is AArch64
 */
lower_el_aarch64_sync:
    store_all
    bl      handler_lower_el_aarch64_sync
    load_all
    eret
                         
lower_el_aarch64_irq:
    store_all
    bl      irq_mask
    bl      handler_lower_el_aarch64_irq
    bl      irq_unmask
    load_all
    eret
                         
lower_el_aarch64_fiq:
    eret

lower_el_aarch64_serror:
    eret


/**
 * Exception from a lower EL and all lower Els are AArch32
 */
lower_el_aarch32_sync:
    eret
                         
lower_el_aarch32_irq:
    eret
                         
lower_el_aarch32_fiq:
    eret
                         
lower_el_aarch32_serror:
    eret

