# Lab3 Questions

> Q: Change svc instruction to brk (breakpoint) instruction. See the difference in ELR_EL2(return address). Explain why there is a difference.
A: brk points to the instruction triggered, svc points to the next instruction. For example, when an SVC (system call) instruction is executed, you want to return to the following instruction in the application. In other cases, however, you might want to re-execute the instruction that generated the exception.

Ref: https://developer.arm.com/docs/100933/0100/returning-from-an-exception

