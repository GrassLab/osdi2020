/* #include <stdio.h> */
#include <fcntl.h>
#include <unistd.h>

#include "memset.h"
#include "shell.h"
#include "uart.h"

int getcmd(char *buf, int nbuf) {
  uart_puts("# ");

  memset(buf, 0, nbuf);
  char *p_buf = buf;

  /* read from uart to buf until newline */
  char c;
  while ((c = uart_getc()) != '\r') {
    if (c == 127 || c == 8) { /* backspace or delete */
      /* display */
      if (p_buf != buf) {
        /* display */
        uart_puts("\b \b");
        /* modified the buffer */
        *buf-- = 0;
      }
    } else {
      uart_send(c);
      *buf++ = c;
    }
  }
  uart_puts("\r\n");
  *buf = 0;

  return p_buf[0] == 0 ? -1 : 0;
}
