
int strcmp(const char* s1, const char* s2){
    while (*s1 && (*s1 == *s2))
        s1++, s2++;
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

char *strchr(const char *s, int c){
    while (*s != (char)c)
        if (!*s++)
            return 0;
    return (char *)s;
}

unsigned int strlen(const char *s) {
    const char *p = s;
    while (*s) ++s;
    return s - p;
}
