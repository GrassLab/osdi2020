#include <stddef.h>
#include "kernel/lib/string.h"
#include "kernel/lib/utils.h"

void *memcpy(void *dest, const void *src, size_t n) {
  for (size_t i = 0; i < n; ++i) {
    ((uint8_t *)dest)[i] = ((uint8_t *)src)[i];
  }
  return dest;
}

void *memset(void *s, int c, size_t n) {
  for (size_t i = 0; i < n; ++i) {
    ((uint8_t *)s)[i] = c;
  }
  return s;
}

int strcmp(const char *s1, const char *s2) {
  for (; *s1 != '\0' && *s1 == *s2; ++s1, ++s2);
  return *s1 - *s2;
}

char *strcpy(char *dest, const char *src) {
  size_t i = 0;
  for (; src[i] != '\0'; ++i) {
    dest[i] = src[i];
  }
  dest[i] = src[i];
  return dest;
}

size_t strlen(const char *s) {
  size_t len = 0;
  for (; s[len] != '\0'; ++len);
  return len;
}

char *strtrim(char *s) {
  char *begin = s;
  for (; isspace(*begin); ++begin);
  char *end = begin + strlen(begin) - 1;
  for (; end > begin && isspace(*end); --end);
  *(end + 1) = '\0';
  return begin;
}

char *strtok(char *str, const char *delim) {
  static char *last_pos;
  if (str != NULL) {
    last_pos = str;
  }
  if (*last_pos == '\0') {
    return NULL;
  }
  char *tmp = last_pos;
  for (; *last_pos != '\0'; ++last_pos) {
    for (int i = 0; i < strlen(delim); ++i) {
      if (*last_pos == delim[i]) {
        *last_pos = '\0';
        ++last_pos;
        return tmp;
      }
    }
  }
  return tmp;
}
