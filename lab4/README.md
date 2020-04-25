# Lab 4 : Multitasking

## Requirement 1
### Design your own task struct
- [ ] `required 1-1` Design your own task struct with at least one field called task id. Task ids should be unique.

### Create privilege tasks
- [ ] `required 1-2` Implement `privilege_task_create`.

### Context Switch
- [ ] `required 1-3` Implement `context_switch`.

### Put task into runqueue
- [ ] `required 1-4` In the end of `privilege_task_create`, put the task into the runqueue.

### Schedule a task
- [ ] `required 1-5` Replace `context_switch` with `schedule` in the privilege tasks, you should be able to create more than 2 tasks and switch between them.


## Requirement 2

### Timer Interrupt
- [ ] `required 2-1` Reimplement the core timer handler, it updates the current task’s reschedule flag.
- [ ] `required 2-2` Create more than 2 privilege tasks. Each of them keeps checking the reschedule flag. If the flag is set, it prints some message, clears reschedule flag and issue `schedule` .


## Requirement 3

### do_exec
- [ ] `required 3-1` Implement `do_exec`

### User tasks preemption
- [ ] `required 3-2` Implement user tasks preemption.


## Requirement 4

### I/O system calls
- [ ] `required 4-1` Implement `uart_read` and `uart_write`.

### Exec
- [ ] `required 4-2` Implement `exec`.

### Fork
- [ ] `required 4-3` Implement `fork`.

### Zombie reaper
- [ ] `required 4-4` Implement exit and zombie reaper by one of the above methods.



## Elective

###　Signal
- [ ] `elective 1` Implement `kill` and signal handler for SIGKILL.

- [ ] `question 1` Consider the following POSIX signal example code. Can you elaborate how to design the kernel to support it?

### Priority based scheduler
- [ ] `elective 2` Implement a priority based scheduler.

### Wait for event

- [ ] `elective 3` Implement a wait queue for uart reading.

### Lock
- [ ] `elective 4` Implement `mutex_lock`, `mutex_unlock`. If task fail to acquire the lock, it would go to sleep and context switch to other tasks

- [ ] `question 2` Can you prevent all possible context switch by disabling interrupt?

### Preemtive kernel

- [ ] `elective 5` Let kernel could be preempted without explicit calling schedule.

- [ ] `question 3` Do you think microkernel need to be preemptive kernel or not? Why or why not?
