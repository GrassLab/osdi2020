#ifndef _LINUX_STRING_H_
#define _LINUX_STRING_H_

typedef unsigned long size_t;
int strcmp (const char *cs, const char *ct);
int strncmp (const char *s1, const char *s2, size_t n);
void *memcpy (void *dest, const void *src, size_t len);
void *memset (void *dest, int val, size_t len);
void bzero (void *s, size_t len);
long strtol (const char *__restrict nptr, char **__restrict endptr, int base);

#endif /* _LINUX_STRING_H_ */
