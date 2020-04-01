#pragma once

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

int ceil(float num);
char *itoa(int val, int base);
int sstrcmp(const char *s1, const char *s2);
void *memset(void *s, int c, size_t n);
void memcpy(void *dest, const void *src, size_t num);
void uart_println(char *format, ...);
void  __attribute__((noreturn)) abort();
