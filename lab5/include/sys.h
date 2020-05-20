#ifndef	_SYS_H
#define	_SYS_H

#define __NR_syscalls	    4

#define SYS_WRITE_NUMBER    5 		// syscal numbers 
#define SYS_READ_NUMBER     6
#define SYS_EXEC_NUMBER     7
#define SYS_FORK_NUMBER     8	
#define SYS_EXIT_NUMBER     9

#ifndef __ASSEMBLER__

void do_uart_write(char *arg);
void do_uart_read(char *line);
void do_exec(void(*func)());
int do_fork();

#endif
#endif  /*_SYS_H */