/*
 * string.c
 * string manipulation utilities
 */

#include "types.h"

size_t strlen(const char *s) {
    const char *p;
    for (p = s; *p; ++p);
    return p - s;
}

int strcmp(const char *s1, const char *s2) {
    while(*s1 && *s1 == *s2)
        s1++, s2++;
    return *s1 - *s2;
}

void reverse(char *s) {
    size_t i, j;
    char tmp;
    for(i = 0, j = strlen(s) - 1; i<j; i++, j--) {
        tmp = *(s+i);
        *(s+i) = *(s+j);
        *(s+j) = tmp;
    }
}

void itoa(int n, char *s) {
    int neg = (n < 0), i = 0;
    if (neg)
        n = -n;
    do {
        s[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);
    if (neg)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);
}

void ftoa(float f, char *s) {
    int n = (int)f;
    float fp = f - (float)n;
    fp *= 100000; // fixed precision
    itoa(n, s);
    int i = strlen(s);
    *(s+i) = '.';
    itoa((int)fp, s + i + 1);  
}

void memcpy(void *dest, void *src, size_t n) {
   char *csrc = (char *)src;
   char *cdest = (char *)dest;

   for (int i=0; i<n; i++)
       cdest[i] = csrc[i];
}
