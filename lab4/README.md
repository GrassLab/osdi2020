# Lab 4 : Multitasking

## Requirement 1
### Design your own task struct
- [x] `required 1-1` Design your own task struct with at least one field called task id. Task ids should be unique.

### Create privilege tasks
- [x] `required 1-2` Implement `privilege_task_create`.

### Context Switch
- [x] `required 1-3` Implement `context_switch`.

### Put task into runqueue
- [x] `required 1-4` In the end of `privilege_task_create`, put the task into the runqueue.

### Schedule a task
- [x] `required 1-5` Replace `context_switch` with `schedule` in the privilege tasks, you should be able to create more than 2 tasks and switch between them.


## Requirement 2

### Timer Interrupt
- [x] `required 2-1` Reimplement the core timer handler, it updates the current task’s reschedule flag.
- [x] `required 2-2` Create more than 2 privilege tasks. Each of them keeps checking the reschedule flag. If the flag is set, it prints some message, clears reschedule flag and issue `schedule` .


## Requirement 3

### do_exec
- [ ] `required 3-1` Implement `do_exec`

### User tasks preemption
- [x] `required 3-2` Implement user tasks preemption.


## Requirement 4

### I/O system calls
- [x] `required 4-1` Implement `uart_read` and `uart_write`.

### Exec
- [x] `required 4-2` Implement `exec`.

### Fork
- [x] `required 4-3` Implement `fork`.

### Zombie reaper
- [x] `required 4-4` Implement exit and zombie reaper by one of the above methods.



## Elective

###　Signal
- [ ] `elective 1` Implement `kill` and signal handler for SIGKILL.

- [ ] `question 1` Consider the following POSIX signal example code. Can you elaborate how to design the kernel to support it?

#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void handler(int sig) {
  printf("Hello\n");
}

int main()
{
  signal(SIGINT, handler);
  char buf[256];
  int n = read(0, buf, 256);
  buf[n] = '\0';
  printf("Bye %s\n", buf);
}

當用戶希望中斷行程時，SIGINT訊號由用戶的控制終端傳送到行程。這通常通過按下Ctrl+C來傳送，但是在某些系統中，可以使用「DELETE」鍵或「BREAK」鍵。

第一種是類似中斷的處理常式，對於需要處理的信號，進程可以指定處理函數，由該函數來處 理。
第二種方法是，忽略某個信號，對該信號不做任何處理，就象未發生過一樣。
第三種方法是，對該信號的處理保留系統的預設值，這種缺省操作，對大部分的信 號的缺省操作是使得進程終止。進程通過系統調用signal來指定進程對某個信號的處理行為。

研究handler做了什麽


### Priority based scheduler
- [ ] `elective 2` Implement a priority based scheduler.

### Wait for event

- [ ] `elective 3` Implement a wait queue for uart reading.

### Lock
- [ ] `elective 4` Implement `mutex_lock`, `mutex_unlock`. If task fail to acquire the lock, it would go to sleep and context switch to other tasks

- [ ] `question 2` Can you prevent all possible context switch by disabling interrupt?
照理說沒有其他訊號打進來就可以

舉例不能打進來的，或是講出可以佔據的。

### Preemtive kernel

- [ ] `elective 5` Let kernel could be preempted without explicit calling schedule.

- [x] `question 3` Do you think microkernel need to be preemptive kernel or not? Why or why not?
挑戰
因為所有的系統服務都是透過IPC呼叫，所以IPC是整個系統的效能關鍵之一，事實上μ-kernel-based的系統在呼叫系統服務上的overhead會大於傳統系統，μ-kernel based的系統需要四個mode的切換以及兩次完整的context switch；傳統系統只需要兩個mode切換且不需要context switch。設計並且實作μ-kernel的挑戰就在盡力降低IPC的overhead上。

Micro kernel 微内核的基本原理是，只有最基本的操作系统功能才能放在内核中。不是最基本的服务和应用程序在微内核之上构造，并在用户模式下运行。