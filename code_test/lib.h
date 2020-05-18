#ifndef _LIB_H_
#define _LIB_H_
#include "common.h"

double gettime();
void core_timer();
int fork();
int exec();
void exit(int value);
int get_taskid();
void reboot();
void kill(int task_id, int signal);
int remain_page_num();

void uart_send(unsigned int c);
char uart_recv();

char uart_getc();
void uart_puts(char *s);

void uart_send_int(int);
void uart_send_hex(unsigned int);

int uart_gets(char *buf, int buf_size);

void putc(void *p, char c);

int strcmp(const char* s1, const char* s2);
void *memset(void *str, int c, size_t n);
void * memcpy (void *dest, const void *src, size_t len);
#endif