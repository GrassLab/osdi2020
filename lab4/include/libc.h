#pragma once

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

char *itoa(int val, int base);
size_t strlen(const char *str);
char *strcat(char *dest, const char *src) ;
int sstrcmp(const char *s1, const char *s2);
void *mmemset(void *s, int c, size_t n);
void mmemcpy(void *dest, const void *src, size_t num);
void uart_println(char *format, ...);
void uart_print(char *format, ...);
void sys_println(char *format, ...);
void sys_print(char *format, ...);
void  __attribute__((noreturn)) abort();
