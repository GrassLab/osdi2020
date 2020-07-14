# Lab 1: Hello World

## Requirement
- Basic initialization

  ✅ Implement the 3 basic steps.
- Mini UART

  ✅ Following UART to set up mini UART.
- Simple Shell

  ✅ Implement a simple shell, it should support the following commands.

## Question
### Is it reasonable to accelerate booting speed by parallel programming during the initialization stage?
It lacks a scheduler to synchronize each parallel things, eg. thread. However, instruction level seems possible for parallel programming like in BSS initialization.
### Point out the difference between bare-metal programming and programming on top of operating system.
bare-metal programming is closer to hardware and usually means creating a BIOS or a Boot Loader.