#ifndef   _SYS_H
#define _SYS_H

#define __NR_syscalls       11

#define SYS_GET_TASKID_NUMBER 0 
#define SYS_FORK_NUMBER       1   
#define SYS_EXIT_NUMBER       2   
#define SYS_EXEC_NUMBER       3
#define SYS_UART_READ_NUMBER  4
#define SYS_UART_WRITE_NUMBER 5
#define SYS_REGISTER_ALLOCATOR 6
#define SYS_REQ_ALLOCATE 7
#define SYS_FREE_ALLOCATE 8
#define SYS_MALLOC 9
#define SYS_FREE 10

#ifndef __ASSEMBLER__


#endif
#endif  /*_SYS_H */
