OSDI LAB3
===

Lab3
===
#### `R1-0` Set up the exception vector table.

#### `R1-1` Implement the exception handler for Synchronous Exceptions from the currentEL while using SP_ELx (offset 0x200-0x280 in the vector table).
- Register
	- currentEL
		- Holds the current Exception level.
	- SP_ELx
		- Holds the stack pointer associated with ELx

el0 不會處理 exception，所以如果在 el0 裡面遇到 exception，會先跳回 el1，才去執行 exception。

#### `R1-2` Add an exc command to the shell. It issues svc #1 and then your exception handler should print the return address, EC field, and ISS field.
- Instruction
	- svc (supervisor call)
		- kernel mode 和 user mode 的橋樑。是 exception 的一種，且為 hardware exception。
- Registers
	- ELR_ELx (Exception Link Register)
		-  holds the address to return to after an exception.
	- ESR_ELx (Exception Syndrome Register)
		- Contains information which allows the exception handler to determine the reason for the exception.
		- Holds syndrome information for an exception taken to ELx.

> exc
```
Exception return address 0x830C8
Exception class (EC) 0x15
Instruction specific syndrome (ISS) 0x1
```
#### `Q1` Change svc instruction to brk (breakpoint) instruction. See the difference in ELR_EL2(return address). Explain why there is a difference.
- Instruction
	- brk (Breakpoint)
		- Generates a Breakpoint Instruction exception. 

> svc #1
```
Exception return address 0x82B20
Exception class (EC) 0x15
Instruction specific syndrome (ISS) 0x1
```

> brk #1
```
Exception return address 0x82ADC
Exception class (EC) 0x3C
Instruction specific syndrome (ISS) 0x1
Exception return address 0x82ADC
Exception class (EC) 0x3C
Instruction specific syndrome (ISS) 0x1
...
```

svc 的 ELR_EL2 儲存的是下一行 instruction 的 address；<br>
brk 的 ELR_EL2 儲存的是同一行 instruction 的 address，所以會不斷的執行 exception。

#### `R2` Remove the infinite loop in exception_handler function and add eret at the end of ISRs. Observe the difference between saving and not saving general registers.
- Instruction
	- eret (exception return)

> svc #1
```
Exception return address 0x830C8
Exception class (EC) 0x15
Instruction specific syndrome (ISS) 0x1
```
用 eret 的時候，return address 會儲存在 ELR_ELx 中；在使用 ret 的時候，return address 會儲存在 LR(x30) 中。<br>
如果在做 exception 前不儲存 registers 的話，registers 可能會不小心被改變，如此一來，做完 exception 後就無法回到本來的 status 了。

#### `Q2` Do you need to save floating point SIMD registers in ISRs? Why or why not.
有用到就要儲存，沒有用到就不用儲存。

#### `Q3` What will happen if you don’t clear peripherals’ interrupt signal?
會以為有新的 interrupt 進來，所以會不斷地執行 interrupt。比方說如果不把 timer interrupt disable 掉的話，就會不斷的收到 interrupt，不斷的 trigger 有可能造成當機。


#### `R3-1` Implement IRQ handler for IRQ Exception from the current EL while using SP_ELx. (offset 0x280-0x300 in the vector table)
- Instruction
	- hcr_elx (Hypervisor Configuration Register)
		- Provides configuration controls for virtualization, including defining whether various operations are trapped to ELx.

#### `R3-2` Implement the arm core timer handler. Add irq command to the shell to enable timer.
- Instruction
	- CNTFRQ_ELx (Counter-timer Frequency)
		- Software can discover the frequency of the system counter.
	- CNTP_CTL_ELx (Counter-timer Physical Timer Control)
		- Control register for the EL1 physical timer.
	- CNTP_TVAL_ELx (Counter-timer Physical Timer TimerValue)
		- Writing a time period, it automatically sets CNTP_CVAL_EL0 according to CNTPCT_EL0.
		- Holds the timer value for the ELx physical timer.
	- CNTPCT_ELx
		-  Current timer count.
	- CNTP_CVAL_ELx
		- If CNTPCT_EL0 >= CNTP_CVAL_EL0 raise interrupt.

CNTP_CVAL_EL0 當 CNTPCT_EL0 >= CNTP_CVAL_EL0 成立時，會發起 timer interrupt；而 CNTP_TVAL_EL0 則會自動更新 CNTP_CVAL_EL0。所以在 core timer handler 裡我們可以使用 CNTP_CVAL_EL0，也可以使用 CNTP_TVAL_EL0。

#### `E1` Pick another timer and implement its handler.
Local timer interrupt

#### `E2` Implement ISR for either mini UART or PL011 UART.
只實作了 keyboard 的 rx, tx。

#### `E3` Use a long delay to simulate bottom half of ISR. Compare the difference between enabling and not enabling interrupt after top half of ISR.
I didn't do this.

#### `R4`
printf 在 el1 會爆炸
Solution => 在 makefile 裡的 CFLAG 加上 `-mgeneral-regs-only`，避免改動到一些 FLAG

### Reference
- [arm Developer Document](https://developer.arm.com/docs/100026/0102/introduction)
- [Application Note / Bare-metal Boot Code for ARMv8-A Processors / Version 1.0]()
- [ARMv8 - 成大資工Wiki](http://wiki.csie.ncku.edu.tw/embedded/ARMv8)
- [學習實作小型作業系統 (筆記)](https://hackmd.io/@tina0405/S1Ab0A9pX?type=view)
