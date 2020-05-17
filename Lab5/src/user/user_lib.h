#ifndef	_USER_LIB_H
#define	_USER_LIB_H

#define NULL ((char *)0)
//prot
#define PROT_NONE 0b000  // non-executable page frame for EL0
#define PROT_READ 0b100  // rwx bit represent
#define PROT_WRITE 0b110  
#define PROT_EXEC 0b101  
//flag
#define MAP_FIXED 0
#define MAP_ANONYMOUS 1

int call_core_timer();
void call_daif();
int fork();
void kill(int pid,int signal);
int exec(void(* func));
void exit(int status);
int get_taskid();
unsigned long uart_write(char *buf, unsigned long size);
unsigned long uart_read(char *buf,unsigned long size);
int printf(char *fmt, ...);
void reboot();
void delay (unsigned long t);
int get_remain_page_num();
void* mmap(void* addr, unsigned long len, int prot, int flags, void* file_start, int file_offset);
#endif  
