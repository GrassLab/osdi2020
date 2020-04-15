# Lab3 Questions

> Q: Change svc instruction to brk (breakpoint) instruction. See the difference in ELR_EL2(return address). Explain why there is a difference.
A: brk points to the instruction triggered, svc points to the next instruction. For example, when an SVC (system call) instruction is executed, you want to return to the following instruction in the application. In other cases, however, you might want to re-execute the instruction that generated the exception.

Ref: https://developer.arm.com/docs/100933/0100/returning-from-an-exception

> Q: Do you need to save floating point SIMD registers in ISRs? Why or why not.
A: Depends. If you need to use floating point or SIMD during ISR then you should save it.

> Q: What will happen if you don’t clear peripherals’ interrupt signal?
A: The local timer counts down and re-loads when it gets to zero. At the same time an interrupt-flag is set.
The user must clear the interrupt flag. There is no detection if the interrupt flag is still set when the next
time the local timer re-loads. The interrupts generated by the timer behave in a level-sensitive manner. This means that, once the timer firing condition is reached, the timer will continue to signal an interrupt until one of the following situations occurs: (a) IMASK is set to one, which masks the interrupt. (b) ENABLE is cleared to 0, which disables the timer. (c) TVAL or CVAL is written, so that firing condition is no longer met.
