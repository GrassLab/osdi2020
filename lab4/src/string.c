
int strcmp(const char *s1, const char *s2) {
  while (*s1 && (*s1 == *s2))
    s1++, s2++;
  return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

char *strchr(const char *s, int c) {
  while (*s != (char)c)
    if (!*s++)
      return 0;
  return (char *)s;
}

unsigned int strlen(const char *s) {
  const char *p = s;
  while (*s)
    ++s;
  return s - p;
}

void *memset(void *s, int c, unsigned long n) {
  unsigned char *p = (unsigned char *)s;

  while (n--)
    *p++ = (unsigned char)c;

  return s;
}

int strbeg(char *str, char *beg){
  while(*beg){
    if(*str == *beg) str++, beg++;
    else return 0;
  }
  return 1;
}

void memcpy(void *dest, const void *src, unsigned long num) {
  char *dest8 = (char *)dest;
  const char *src8 = (const char*)src;
  while (num--) {
    *dest8++ = *src8++;
  }
}
