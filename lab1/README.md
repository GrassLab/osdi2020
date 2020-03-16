# Lab 1 : Hello World+


## todo

### Basic initialization

- [x] [required]Implement the 3 basic steps.
  - Let only one core proceed, and let others enter a busy loop.
  - Initialize the BSS segment.
  - Set the stack pointer to an appropriate position.
- [x] [question]Is it reasonable to accelerate booting speed by parallel programming during the initialization stage?
- Answer: In my opinion, it will cost more when dealing with parallel programming. So it won't be a good idea.

### Mini UART
- [x] [required]Following UART to set up mini UART.

### Simple Shell

- [x] [required]Implement a simple shell, it should support the following commands.
- [] [elective]Write a program or script on your host computer which can read a text file and write the content to rpi3.

###　Get time

- [] [elective]Add <timestamp> command, it print current timestamp.

### Reboot

- [] [elective]Add <reboot> command.

### Result
- [] [question]Point out the difference between bare-metal programming and programming on top of operating system.