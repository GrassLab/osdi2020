#ifndef _LINUX_STRING_H_
#define _LINUX_STRING_H_

#include <stddef.h>
#define STRBUF_SIZE 0x30
#define abort() {while (1);}

int strcmp (const char *cs, const char *ct);
int strncmp (const char *s1, const char *s2, size_t n);
void *memcpy (void *dest, const void *src, size_t len);
void *memset (void *dest, int val, size_t len);
void bzero (void *s, size_t len);
long strtol (const char *__restrict nptr, char **__restrict endptr, int base);
int printf (const char *format, ...);
void ftoa (double val, char *buf);
void ltoa (unsigned long val, char *buf);
void fdivtoa (unsigned long dividend, unsigned long divisor, char *buf);
size_t strlen (const char *str);

#endif /* _LINUX_STRING_H_ */
