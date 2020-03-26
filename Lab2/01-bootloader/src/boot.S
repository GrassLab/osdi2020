
.section ".text.boot"


.globl _start
_start:
    //save arguments in registers
    mov    x10, x0
    mov    x11, x1
    mov    x12, x2
    mov    x13, x3

    mrs    x0, mpidr_el1       // put mpidr_el1(process id) in x0
    and    x0, x0,#0xFF        // Check processor id(x0= x0 and 0xFF)
    cbz    x0, master          // Hang for all non-primary CPU
			       //(if x0 == 0 jump to master)
    b    proc_hang             //Else it will become a for loop

proc_hang: 
    b proc_hang

master:
    
    //relocate code from load address to link address
    ldr    x1, =0x80000
    ldr    x2, =_start
    ldr    w3, =__loader_size
loop:
    ldr    x4, [x1], #8
    str    x4, [x2], #8
    sub    w3, w3, #1
    cbnz   w3, loop

    // set stack point before our code
    ldr    x1, = _start
    mov    sp, x1
    
    // clear bss
    ldr    x1, =_bss_begin
    ldr    w2, =__bss_size
clean:    
    cbz    w2, main 
    str    xzr, [x1], #8
    sub    w2, w2, #1
    cbnz   w2, clean

main:
    bl    kernel_main-2048
	
b proc_hang    // for failsafe, halt this core too         
