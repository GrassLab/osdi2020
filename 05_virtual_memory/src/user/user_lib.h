#ifndef	_USER_LIB_H
#define	_USER_LIB_H

typedef long long size_t;

size_t uart_read(char*, size_t);
size_t uart_write(const char*, size_t);
int exec(unsigned long);
int fork();
void exit(int);

#endif  
