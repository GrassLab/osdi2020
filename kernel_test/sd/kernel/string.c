int strcpy(const char *ct, const char *cs) 
{
    char* dst = ct;
    char* src = cs;
    while (*src != 0) {
        *dst++ = *src++;
    }
    return 0;
}

int strncpy(const char *ct, const char *cs, int len)
{
    char* dst = ct;
    char* src = cs;
    while (len--) {
        *dst++ = *src++;
    }
    return 0;
}

int strcmp(const char *ct, const char *cs)
{
    unsigned char c1, c2;
    while (*cs != 0) {
        c1 = *cs++;
        c2 = *ct++;
        if (c1 != c2) return c1 < c2 ? -1 : 1;
        if (!c1) break;
    }
    return 0;
}

/**
 * Compare two string and return 0 if they are identical
 */ 
int strncmp(const char *ct, const char *cs, int len)
{
    unsigned char c1, c2;
    while (len--) {
        c1 = *cs++;
        c2 = *ct++;
        if (c1 != c2) return c1 < c2 ? -1 : 1;
        if (!c1) break;
    }
    return 0;
}