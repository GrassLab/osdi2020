#include "kernel/lib/string.h"
#include "kernel/lib/utils.h"

int32_t ctoi(char c) {
  if (c >= 'A' && c <= 'F') {
    return 10 + (c - 'A');
  } else if (c >= 'a' && c <= 'f') {
    return 10 + (c - 'a');
  } else if (c >= '0' && c <= '9') {
    return c - '0';
  } else {
    return -1;
  }
}

int isspace(char c) {
  return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

uint64_t pow(uint32_t base, uint32_t exponent) {
  uint64_t rst = 1;
  for (uint32_t i = 0; i < exponent; ++i) {
    rst *= base;
  }
  return rst;
}

uint64_t stoui(const char *s, int base) {
  uint64_t num = 0;
  for (int32_t i = strlen(s) - 1, j = 0; i >= 0; --i, ++j) {
    int32_t val = ctoi(s[i]);
    if (val < 0) {
      return num;
    } else {
      num += val * pow(base, j);
    }
  }
  return num;
}

char *uitos_generic(uint64_t num, int base, char *buf) {
  const char *alphabet = "0123456789abcdef";
  char *cur = buf;
  uint8_t len = 0;

  do {
    *cur = alphabet[num % base];
    num /= base;
    ++cur;
    ++len;
  } while (num > 0);
  *cur = '\0';

  for (int i = 0; i < len / 2; ++i) {
    char tmp = buf[len - i - 1];
    buf[len - i - 1] = buf[i];
    buf[i] = tmp;
  }

  return buf;
}

char *uitos(uint64_t num, char *buf) {
  uitos_generic(num, 10, buf);
}
