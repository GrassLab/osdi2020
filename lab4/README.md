# Multitasking

## Introduction
You’ll learn how to implement the basis of multitasking and understand how user mode and kernel mode interact in depth.

## Memory Layout
```
+-----------------+ Low
|       ...       |
+-----------------+ 
|    EL1 stack    |
+-----------------+ 0x80000
|   kernel code   |
+-----------------+ __bss_start
|   task pcb[0]   |
+-----------------+
|       ...       |
+-----------------+
|  task pcb[N-1]  |
+-----------------+
| kernel stack[0] |
+-----------------+
|       ...       |
+-----------------+
|kernel stack[N-1]|
+-----------------+
|  user stack[0]  |
+-----------------+
|       ...       |
+-----------------+
| user stack[N-1] |
+-----------------+
|       ...       |
+-----------------+ High
```

## Privilege tasks & User tasks
### Privilege task
* always run in EL1
* using kernel stack

### User tasks
* always run in EL0
* using user stack
* call system call to enter EL1
* Has its own user mode’s context and the user mode’s context should be saved and restored when enter and exit the kernel mode.

### In Common
* Have its own kernel stack.
* Context switch in kernel mode.
* Queue in the same runqueue.

## Runtime
![runtime_state](https://github.com/catvmzhang/osdi2020/blob/0756107/pic/runtime_state.png)
*From OSDI course TA*

## Task required work
### privilege_task_create(void(*func)())
* create task structure e.g. taskid...
* allocate task PCB and user/kernel stack
### context_switch(struct task* next)
* to switch between each task. You need to save the current task’s context and restore the next task’s context.
* we must to preserve x19 ~ x30 and sp register [ref](https://en.wikichip.org/wiki/arm/aarch64)
### Scheduler
* create run queue and process state
* idle task
  * Setup the kernel after booted by GPU.
  * Call privilege_task_create to create the second task.
  *Enter idle state, and now the startup task becomes the idle task.
  * In idle state, repeatly call schedule in an infinite loop, the idle task get switched out if there is another runnable task.
* can be triger by timer interrupt
* user mode
  * SP_EL0: The address of user mode’s stack pointer.
  * ELR_EL1: The program counter of user mode’s procedure.
  * SPSR_EL1: The CPU state of user mode.
### do_exec(void(*func)())
* takes a function pointer to user code. And it should set up the task’s user context.

## System Call
* When user task enter kernel mode, it saves registers on the top of its kernel stack. We name it as **trapframe**
  * User can set registers to be parameters and system call number. After system call into kernel, kernel can read the content from trapframe.
  * Kernel can set the content of trapframe. After return to user, user get the return value and error number from the restored registers.
### UART read/write
* using UART to read and write without any possible to be interrupted by other process.
### Exev
* call do_exec(void(*func())
### fork
* parent process return child process id
* child process return 0
### Exit
* release most of its resource but keepping the kernel stack and task struct. Then, it set its state to be zombie state and won’t be scheduled again.
### Zombie reaper
* A task becomes zombie after it exit, a zombie reaper is here to reclaim the remaining allocated resource for zombie task.
