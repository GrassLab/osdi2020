#include "uart.h"

int my_strcpy(char *buf1, char *buf2) {
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

void compair(char *buf) {
  if (my_strcpy(buf, "hello")) {
    uart_puts("Hello World!\n");
  } else if (my_strcpy(buf, "help")) {
    uart_puts("help : print all available commands.\n");
    uart_puts("hello : print Hello World \n");
  } else {
    uart_puts("unknow command ");
    uart_puts(buf);
    uart_puts(". \n");
  }
}

void clearbuf(char *buf) {
  for (int i = 0; i < 100; i++) {
    *(buf + i) = 0;
  }
}
void main() {
  uart_init();
  uart_puts("# ");

  char commandbuf[100];
  clearbuf(commandbuf);
  int count = 0;
  while (1) {
    char get = uart_getc();
    uart_send(get);
    if (get != '\n') {
      commandbuf[count] = get;
      count++;
    } else {
      commandbuf[count] = '\x00';
      compair(commandbuf);
      uart_puts("# ");
      count = 0;
      clearbuf(commandbuf);
    }
  }
}
