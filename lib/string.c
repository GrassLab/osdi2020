#include <stddef.h>
#include "string.h"
#include "utils.h"

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
