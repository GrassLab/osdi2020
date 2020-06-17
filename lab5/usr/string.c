void *memcpy(void *dest, const void *src, unsigned int len) {
  char *d = dest;
  const char *s = src;
  while (len--)
    *d++ = *s++;
  return dest;
}

int strcpy(char *buf1, char *buf2) {
  int i;
  for (i = 0; *(buf1 + i) != '\x00'; i++) {
    if (*(buf1 + i) != *(buf2 + i)) {
      return 0;
    }
  }
  if (*(buf2 + i) != '\x00') {
    return 0;
  }
  return 1;
}

void clearbuf(char *buf, int size) {
  for (int i = 0; i < size; i++) {
    *(buf + i) = 0;
  }
}