.global kernel_recv
kernel_recv:
    // x0 = size
    // x1 = load addr
    mov x2, x1          // copy load addr as kernel entry 
    ldr w7, =0x3F201018 // UART_FD
    ldr w6, =0x3F201000 // UART_DR
wait_DR:
    ldr w5, [x7]        // w5=UART_FR
    and w5, w5, #0x10   // UART_REG-> FR & UART_FR_RXFE
    cmp w5, wzr 
    bne wait_DR         // wait_DR
    ldrb w4,[x6]        // w4=UART_REG-> DR
    strb w4, [x1], #1   // load_addr[i] = w4
    sub w0, w0, #1      // size --
    cmp w0, wzr 
    bne wait_DR         // load next char
    br x2               // jump to _start
