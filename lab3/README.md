# Lab 3 : Exception and Interrupt


## todo

## ARM Exception
### Your first exception

- [x] `required 1-0` Set up the exception vector table.
- [x] `required 1-1` Implement the exception handler for Synchronous Exceptions from the currentEL while using SP_ELx (offset 0x200-0x280 in the vector table).
- [x] `required 1-2` Add an `exc` command to the shell. It issues `svc #1` and then your exception handler should print the return address, EC field, and ISS field.
- [x] `question 1` Change `svc` instruction to `brk` (breakpoint) instruction. See the difference in ELR_EL2(return address). Explain why there is a difference.
svc: 0x00081344
because we setup elr_el2 register, so it will retrun to previous work.
brk: 0x00081ED8
return from register X30 is used to return from subroutines
create infinite loop
[ref](https://developer.arm.com/docs/100933/0100/returning-from-an-exception)
### Context saving
- [x] `required 2` Remove the infinite loop in exception_handler function and add `eret` at the end of ISRs. Observe the difference between saving and not saving general registers.

- [x] `question 2` Do you need to save floating point SIMD registers in ISRs? Why or why not.
在exception情況下無法回覆Pipeline的東西，所以存了也沒用

## Pi3’s interrupt
### Background
- [x] `question 3` What will happen if you don’t clear peripherals’ interrupt signal?
無法觸發
The interrupt from a core’s timer can only be delivered to that core. This means that the timer of one core cannot be used to generate an interrupt that targets a different core.

### Timer interrupt
- [x] `required 3-1` Implement IRQ handler for IRQ Exception from the current EL while using SP_ELx. (offset 0x280-0x300 in the vector table)

- [x] `required 3-2` Implement the arm core timer handler. Add `irq` command to the shell to enable timer.

- [x] `elective 1` Pick another timer and implement its handler.

### UART interrupt

- [ ] `elective 2` Implement ISR for either mini UART or PL011 UART.

### Deferred interrupt handle

- [ ] `elective 3` Use a long delay to simulate bottom half of ISR. Compare the difference between enabling and not enabling interrupt after top half of ISR.

## Exception Level Switch

### From EL2 to EL1

- [x] `required 4-1` Return from EL2 to EL1 and set the corresponding handlers.

### EL1 to EL0

- [x] `required 4-2` Return from EL1 to EL0 and run shell in EL0.

### Basic System Call

- [x] `required 4-3` Reimplement `irq` command by system call.


