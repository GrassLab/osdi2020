#include "uart.h"

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

void itoa(long result) {
  long m = result;
  char ms[20] = {0};
  int count;
  for (count = 0; m; count++) {
    ms[count] = (m % 10) + '0';
    m = m / 10;
  }
  int i;
  char rms[20] = {0};
  for (i = count; i > 0; i--) {
    rms[count - i] = ms[i - 1];
  }
  uart_puts(rms);
}

void clearbuf(char *buf, int size) {
  for (int i = 0; i < size; i++) {
    *(buf + i) = 0;
  }
}