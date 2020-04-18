# Exception and Interrupt

## Introduction
In this lab, we have to implement the exception handler, and system call.<br>
To understand how exception work in ARM processor, some terminology we have to know.<br>

### Exception Level
ARMv8-A CPU has 4 exception levels (EL) for different privilege operations. rpi3’s CPUs run in EL2 after booted by GPU by default.<br>
![exception level](https://encrypted-tbn0.gstatic.com/images?q=tbn%3AANd9GcQzitUTC9PpojgzH6xdrVFNxfoiJWQS0iYtHsw-vDj450VYU5Dr&usqp=CAU)

Higher exception level can execute more instructions, e.g. In EL0, you can not execute "mrs x0, esr_el1;", 
it will cause exception(trap) and jump to exception handler.<br>

This mechanism can prevent from some low-privilege process trying to access system register. With this mechanism, we can distinguish 
user process and kernel process.<br>

### NOTE
1. Synchronous exception: means the exception caused by instruction, so it is synchronous with CPU clock.
2. IRQ exception: you can regarded as interrupt, caused by other hardware like timer or other pheripheral.

#### NOTE
**vbar_elx** register is used to store the base address of exception table. When exception level occur, processor will jump to the address
stored in this register and give it a offset in some condiction.<br>
![exception table](http://images4.programmersought.com/822/ec/ec9a3806783b0275f8048870398ada5e.png)

### Switch Exception Level
There are two cases to increase exception level, inactive or active.<br>
**1. Inactive**<br>
When taking excepetion, the exception level will increase. This mechanism is easy to understand. Exception means some problem occur, we 
should use higher privilege exception level process to handle it.<br>
But what if no problem occcur, we just want to increase the exception level? The answer is case two.<br>
**2. Active**<br>
Use instruction to make exception happen. e.g. "svc" instruction. <br>
"svc #1" means supervisor call, the argument "#1" can be regarded as "exception number" for exception handler.<br>
<br>
To decrease the exception level, we can just use "eret" instruction to leave exception.<br>
<br>
#### NOTE
Different exception level use different stack pointer, status register, return address..., so make sure set up well befroe switch.<br>
1. spsr_elx<br>
2. elr_rlx<br>
3. sp_elx<br>
#### NOTE
When taking excepetion, user process is terminate inactive though it use svc instruction. So make sure saving the context of 
user process well at the begining of exception handler, and restore before "eret".

## lab goal
1. In synchronous exception, show the ES field, ISS field and return address.
  <br>* El2 exception
  <br>* vector table design
  <br>* exception context switch
2. Use system timer to implement IRQ handler.
  <br>* timer setting
  <br>* timer interrupt status enable/disable 
3. After booting, make shell run in EL0 and implement system call.
  <br>* different exception level stack
  <br>* system call table
  <br>* spsr cpsr(PSTATE) different
  <br>* enable/disable interrupt
