#ifndef	_SYS_H
#define	_SYS_H

#define __NR_syscalls	 1
#define SYS_TIMER    0   // syscal numbers 

#ifndef __ASSEMBLER__
void sys_timer();
void syscall_success_message();
void print_this();

void call_sys_timer(char *x);
#endif

#endif  /*_SYS_H */
