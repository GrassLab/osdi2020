#include <stddef.h>

void *memcpy(void *dest, const void *src, size_t len) {
    char *d = dest;
    const char *s = src;
    while (len--) *d++ = *s++;
    return dest;
}

int strcmp(const char *p1, const char *p2) {
    const unsigned char *s1 = (const unsigned char *)p1;
    const unsigned char *s2 = (const unsigned char *)p2;
    unsigned char c1, c2;
    do {
        c1 = (unsigned char)*s1++;
        c2 = (unsigned char)*s2++;
        if (c1 == '\0') return c1 - c2;
    } while (c1 == c2);
    return c1 - c2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
    while (n && *s1 && (*s1 == *s2)) {
        ++s1;
        ++s2;
        --n;
    }
    if (n == 0) {
        return 0;
    } else {
        return (*(unsigned char *)s1 - *(unsigned char *)s2);
    }
}

char *strcpy(char *destination, const char *source) {
    // return if no memory is allocated to the destination
    if (destination == NULL) return NULL;

    // take a pointer pointing to the beginning of destination string
    char *ptr = destination;

    // copy the C-string pointed by source into the array
    // pointed by destination
    while (*source != '\0') {
        *destination = *source;
        destination++;
        source++;
    }

    // include the terminating null character
    *destination = '\0';

    // destination is returned by standard strcpy()
    return ptr;
}

char *strncpy(char *destination, const char *source, size_t len) {
    // return if no memory is allocated to the destination
    if (destination == NULL) return NULL;

    // take a pointer pointing to the beginning of destination string
    char *ptr = destination;

    // copy the C-string pointed by source into the array
    // pointed by destination
    while (*source != ' ') {
        *destination = *source;
        destination++;
        source++;
        len--;
        if (!len) {
            break;
        }
    }

    // include the terminating null character
    *destination = '\0';

    // destination is returned by standard strcpy()
    return ptr;
}

size_t strlen(const char *str) {
    const char *s;

    for (s = str; *s; ++s)
        ;
    return (s - str);
}