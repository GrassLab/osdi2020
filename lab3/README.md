# Lab 3 : Exception and Interrupt


## todo

## ARM Exception
### Your first exception

- [ ] [required 1-0] Set up the exception vector table.
- [ ] [required 1-1] Implement the exception handler for Synchronous Exceptions from the currentEL while using SP_ELx (offset 0x200-0x280 in the vector table).
- [ ] [required 1-2] Add an `exc` command to the shell. It issues `svc #1` and then your exception handler should print the return address, EC field, and ISS field.
- [ ] [question 1] Change `svc` instruction to `brk` (breakpoint) instruction. See the difference in ELR_EL2(return address). Explain why there is a difference.


### Context saving
- [ ] [required 2] Remove the infinite loop in exception_handler function and add `eret` at the end of ISRs. Observe the difference between saving and not saving general registers.

- [ ] [question 2] Do you need to save floating point SIMD registers in ISRs? Why or why not.

## Pi3’s interrupt
### Background
- [ ] [question 3] What will happen if you don’t clear peripherals’ interrupt signal?

### Timer interrupt
- [ ] [required 3-1] Implement IRQ handler for IRQ Exception from the current EL while using SP_ELx. (offset 0x280-0x300 in the vector table)

- [ ] [required 3-2] Implement the arm core timer handler. Add `irq` command to the shell to enable timer.

- [ ] [elective 1] Pick another timer and implement its handler.

### UART interrupt

- [ ] [elective 2] Implement ISR for either mini UART or PL011 UART.

### Deferred interrupt handle

- [ ] [elective 3] Use a long delay to simulate bottom half of ISR. Compare the difference between enabling and not enabling interrupt after top half of ISR.

## Exception Level Switch

### From EL2 to EL1

- [ ] [required 4-1] Return from EL2 to EL1 and set the corresponding handlers.

### EL1 to EL0

- [ ] [required 4-2] Return from EL1 to EL0 and run shell in EL0.

### Basic System Call

- [ ] [required 4-3] Reimplement `irq` command by system call.


